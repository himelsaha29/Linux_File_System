#include "sfs_api.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "disk_emu.h"
#include "disk_emu.c"

#define MAX_FIZE_NUM 256;
#define MAX_FILE_NAME 16;
#define SUPERBLOCK 0;
#define BLOCKSIZE 2048;
#define SYSTEMNAME "OS.sfs";

DirectoryEntry rootDirectory[500];
FileDescriptorTable fdtable[500];


typedef struct {
    int magic;
    int blockSize;
    int fileSystemSize;
    int iNodeTable;
    int rootDirectory;

} superBlock;

typedef struct {
    int size;
    int mode;
    int linkCount;
    int uid;
    int gid;

    int pointer1;
    int pointer2;
    int pointer3;
    int pointer4;
    int pointer5;
    int pointer6;
    int pointer7;
    int pointer8;
    int pointer9;
    int pointer10;
    int pointer11;
    int pointer12;
    int indPointer;

} iNode;

typedef struct {
    int i_node;
    char file_name[MAX_FILE_NAME];
} DirectoryEntry;

typedef struct {
    int inodeNumber;
    iNode* inode;
    int rwpointer;
} FileDescriptorTable;




void mksfs(int) {
	
	if(fresh == 1)  { // create file system


  		for (int i = 0; i < 500; i++) { // populate FDTable
  			fdtable[i].inodeNumber = -1;
  			fdtable[i].rwpointer = -1;
 		}


  		for (int i = 0; i < 500; i++) {
  			iNodeTable[i].mode = -1;
  			iNodeTable[i].linkCount = -1;
  			iNodeTable[i].uid = -1;
  			iNodeTable[i].gid = -1;
  			iNodeTable[i].size = -1;
        		iNodeTable[i].pointer1 = -1;
        		iNodeTable[i].pointer2 = -1;
        		iNodeTable[i].pointer3 = -1;
        		iNodeTable[i].pointer4 = -1;
        		iNodeTable[i].pointer5 = -1;
        		iNodeTable[i].pointer6 = -1;
        		iNodeTable[i].pointer7 = -1;
        		iNodeTable[i].pointer8 = -1;
        		iNodeTable[i].pointer9 = -1;
        		iNodeTable[i].pointer10 = -1;
        		iNodeTable[i].pointer11 = -1;
        		iNodeTable[i].pointer12 = -1;
  			iNodeTable[i].indPointer = -1;

  		}

      		for (int i = 0; i < 500; i++)  { // populate root directory

  			rootDirectory[i].iNode = -1;
  		}

      		init_fresh_disk(SYSTEMNAME, BLOCKSIZE, 2048); // creating new file system

      	 	int *superBuffer = malloc(BLOCKSIZE);
      	 	write_blocks(SUPERBLOCK, 1, superBuffer);
      	 	free(superBuffer);

  	 	superBlock.magic = 0;
  	 	superBlock.blockSize = BLOCK_SIZE;
  	 	superBlock.fileSystemSize = 2048;
  	 	superBlock.iNodeTable = 500;
  	 	superBlock.rootDirectory = *rootDirectory;

  	}
	else {
  		int error = init_disk(SYSTEMNAME, BLOCKSIZE, 2048);
      		if (error != 0) {
        		fprintf(stderr, "Error loading file system");
      		}
    	}

}

int sfs_fseek(int fileID, int loc) {
  if (fileID < 0 || loc < 0) {
    fprintf(stderr, "Bad seek argument");
    return -1;
  }

  fdtable[fileID].rwpointer = loc;
  return 0;
}

int sfs_getnextfilename(char* fname) {
    int next = 0;

    for (int i = 0; i < 500; i++)  {
      if(rootDirectory[positionInDir].name == fname) {
          next = i + 1;
          break;
      }
    }
    return next;
}


int sfs_fclose(int fileID) {

	if (fdt[fileID].inodeNumber == -1) {  // file already not being read, cannot close an already closed file
		return -1;
		
	} else {     // else set read/write pointer to invalid numbers
		fdt[fileID].inodeNumber = -1;
		fdt[fileID].rwpointer = -1;
    		fdtable[fileID] = NULL;

	}
	
	return 0;
}


int sfs_fopen(char* name) {
  if(name == NULL){  // throw error on invalid argument
    return -1;
  }
}

int sfs_remove(char *file) {

	int inode = -1;
    	int i;
    	int directory_table_index;
    	for(i = 0; i < sizeof(rootDirectory)/sizeof(rootDirectory[0]); i++) {
	        if(rootDirectory[i].used == 1) {

        	    if (strcmp(rootDirectory[i].name, file) == 0) {
	                inode = rootDirectory[i].inode;
	                directory_table_index = i;
	            }
	        }
    	}

    	if(inode == -1) {
	        printf("File not found!\n");
	        return -1;
    	}

    	// free bitmap
    	inode_t* n = &iNodeTable[inode];
    	int j = 0;
    	while(n->data_ptrs[j] != -1 && j< 12){
	        rm_index(n->data_ptrs[j]);
	        j++;
    	}
    	j = 0;
    	if(n->indirect_ptrs != -1){
	        indirect_t* indirect_pointer = malloc(sizeof(indirect_t));
	        read_blocks(n->indirect_ptrs, 1, (void*) indirect_pointer);
	        while(indirect_pointer->data_ptr[j] != -1 && j< NUM_INDIRECT){
	            rm_index(indirect_pointer->data_ptr[j]);
	            j++;
	        }
    	}

    	// remove from directory_table
    	rootDirectory[directory_table_index].used = 0;

    	// remove from inode_table
    	iNodeTable[inode].used = 0;

    	// update disk
    	uint8_t* free_bit_map = get_bitmap();
	write_blocks(NUM_BLOCKS - 1, 1, (void*) free_bit_map);
    	write_blocks(1, sb.inode_table_len, (void*) inode_table);
    	write_blocks(sb.inode_table_len + 1, NUM_DIR_BLOCKS, (void*) directory_table);

	return 0;
}



int sfs_fread(int fileID, char *buf, int length) {

	char *temp_buf = NULL;
    	int len_temp_buf = 0;
    	if (fdt[fileID].used == 0) {
	        return 0;
    	}


	
    	file_descriptor *f = &fdt[fileID];
    	inode_t *n = &inode_table[f->inode];

	
    	if (f->rwptr+length > n->size){
	        length = n->size - f->rwptr;
    	}
    	int num_block_read = ((f->rwptr % BLOCK_SZ) + length) / BLOCK_SZ + 1;
    	char buffer[num_block_read][BLOCK_SZ];
    	uint64_t data_ptr_index = f->rwptr / BLOCK_SZ;
	
    	int i;
    	for (i = 0; i < num_block_read; i++) {

        	if (data_ptr_index < 12) {
            	read_blocks(n->data_ptrs[data_ptr_index], 1, buffer[i]);
        	}

        	else {
		
            	if (n->indirect_ptrs != -1) {
	                indirect_t* indirect_pointer = malloc(sizeof(indirect_t));
	                read_blocks(n->indirect_ptrs, 1, (void*) indirect_pointer);
	                read_blocks(indirect_pointer->data_ptr[data_ptr_index - 12], 1, buffer[i]);
	            } else {
	                printf("ERROR While reading the file\n");
        	    }
        	}
        	data_ptr_index++;
    	}

    	int start_ptr = f->rwptr % BLOCK_SZ;
    	int end_ptr = (start_ptr + length) % BLOCK_SZ;
	int j;
    	if (num_block_read == 1) {
	        for (j = start_ptr; j < end_ptr; j++) {
	            temp_buf = appendCharToCharArray(temp_buf, buffer[0][j], len_temp_buf);
	            len_temp_buf++;
	        }


    	} else if (num_block_read == 2) {
	        for (j = start_ptr; j < BLOCK_SZ; j++) {
	            temp_buf = appendCharToCharArray(temp_buf, buffer[0][j], len_temp_buf);
	            len_temp_buf++;
	        }
	        for (j = 0; j < end_ptr; j++) {
	            temp_buf = appendCharToCharArray(temp_buf, buffer[1][j], len_temp_buf);
	            len_temp_buf++;
        	}

    	} else {
	        for (j = start_ptr; j < BLOCK_SZ; j++) {
	            temp_buf = appendCharToCharArray(temp_buf, buffer[0][j], len_temp_buf);
	            len_temp_buf++;
	        }
	
	        for (j = 1; j < num_block_read - 1; j++) {
	            int k;
	            for (k = 0; k < BLOCK_SZ; k++) {
	                temp_buf = appendCharToCharArray(temp_buf, buffer[j][k], len_temp_buf);
	                len_temp_buf++;
		    }
        	}

        	for (j = 0; j < end_ptr; j++) {
            		temp_buf = appendCharToCharArray(temp_buf, buffer[num_block_read - 1][j], len_temp_buf);
            		len_temp_buf++;
        	}
    	}


    	f->rwptr += length;
	
    	for(i = 0; i < length; i++) {
		buf[i] = temp_buf[i];
    	}


	return len_temp_buf;
}


int sfs_fwrite(int fileID, const char *buf, int length){

	int count = 0;
    	int length_left = length;
    	file_descriptor* f = &fdt[fileID];
    	inode_t* n = &inode_table[f->inode];
    	int ptr_index = 0;
    	while (n->data_ptrs[ptr_index] != -1 && ptr_index < 12) {
	        ptr_index++;
    	}
    	int ind_ptr_index = 0;


	
    	if (ptr_index == 12 && n->indirect_ptrs != -1) {
	        indirect_t *indirect_pointer = malloc(sizeof(indirect_t));
	        read_blocks(n->indirect_ptrs, 1, (void *) indirect_pointer);
	
	        while (indirect_pointer->data_ptr[ind_ptr_index] != -1) {
	            ind_ptr_index++;
	            if(ind_ptr_index == NUM_INDIRECT){
	                return 0;
	            }
	        }

	    
    	} else if (ptr_index == 12) {
	        indirect_t *indirect_pointer = malloc(sizeof(indirect_t));
	        for (ind_ptr_index = 0; ind_ptr_index < NUM_INDIRECT; ind_ptr_index++) {
	            indirect_pointer->data_ptr[ind_ptr_index] = -1;
	        }
	        n->indirect_ptrs = get_index();
	        write_blocks(n->indirect_ptrs, 1, (void *) indirect_pointer);
	        ind_ptr_index = 0;
    	}


	
    	if(f->rwptr%BLOCK_SZ != 0) {

        	char *read_buf = malloc(sizeof(char)*BLOCK_SZ);

        	uint64_t init_rwptr = f->rwptr;

        	sfs_fseek(fileID, (f->rwptr - (f->rwptr % BLOCK_SZ)));
        	sfs_fread(fileID, read_buf, (init_rwptr % BLOCK_SZ));

        	int len_read_buf = (init_rwptr % BLOCK_SZ);
		int i;

        	if (length_left > BLOCK_SZ - (f->rwptr % BLOCK_SZ)) {
            	for (i = 0; i < BLOCK_SZ - (f->rwptr % BLOCK_SZ); i++) {
	                read_buf = appendCharToCharArray(read_buf, buf[i], len_read_buf);
	                len_read_buf++;
                	count++;
            	}
        	} else {
            		for (i = 0; i < length_left; i++) {
	                	read_buf = appendCharToCharArray(read_buf, buf[i], len_read_buf);
	                	len_read_buf++;
	                	count++;
	            	}
        	}


        	if (ptr_index < 12) {
            		write_blocks(n->data_ptrs[ptr_index-1], 1, (void *) read_buf);
        	}
        	else if(ind_ptr_index == 0){
            		write_blocks(n->data_ptrs[ptr_index-1], 1, (void *) read_buf);
        	}

        	else {
            		if(n->indirect_ptrs != -1) {
                		indirect_t *indirect_pointer = malloc(sizeof(indirect_t));
                		read_blocks(n->indirect_ptrs, 1, (void *) indirect_pointer);
                		write_blocks(indirect_pointer->data_ptr[ind_ptr_index-1], 1, (void *) read_buf);
            		}
        	}

        	length_left -= BLOCK_SZ - (f->rwptr % BLOCK_SZ);
    	}


    	int k;
    	char *write_buf;
    	int len_write_buf;
    	int num_block_left;
    	if(length_left > 0) {
	        num_block_left = (length_left - 1) / BLOCK_SZ + 1;
    	} else if (length_left == 1) {
	        num_block_left = 1;
    	} else {
	        num_block_left = 0;
    	}

    	for(k = 0; k < num_block_left; k++) {
	        write_buf = "";
	        len_write_buf = 0;
    
        	if(length_left > BLOCK_SZ) {

		
            	int w;
            	for(w = length - length_left; w < length - length_left + BLOCK_SZ; w++){
	                write_buf = appendCharToCharArray(write_buf, buf[w], len_write_buf);
	                len_write_buf++;
	                count++;            
		}

            	if(ptr_index < 12) {
	                n->data_ptrs[ptr_index] = get_index();
	                write_blocks(n->data_ptrs[ptr_index], 1, (void*) write_buf);
	                ptr_index++;

            	} else {

                	if(n->indirect_ptrs != -1) {
                    		indirect_t* indirect_pointer = malloc(sizeof(indirect_t));
                    		read_blocks(n->indirect_ptrs, 1, (void*) indirect_pointer);
                    		indirect_pointer->data_ptr[ind_ptr_index] = get_index();
                    		write_blocks(n->indirect_ptrs, 1, (void*) indirect_pointer);
                    		write_blocks(indirect_pointer->data_ptr[ind_ptr_index], 1, (void*) write_buf);
                    		ind_ptr_index++;
                	} else {

                    		indirect_t* indirect_pointer = malloc(sizeof(indirect_t));
                    		for(ind_ptr_index = 0; ind_ptr_index < NUM_INDIRECT; ind_ptr_index++){
                        		indirect_pointer->data_ptr[ind_ptr_index] = -1;
                    		}
                    		n->indirect_ptrs = get_index();
                    		write_blocks(n->indirect_ptrs, 1, (void*) indirect_pointer);
                    		ind_ptr_index = 0;
                    		indirect_pointer->data_ptr[ind_ptr_index] = get_index();
                    		write_blocks(n->indirect_ptrs, 1, (void*) indirect_pointer);
                    		write_blocks(indirect_pointer->data_ptr[ind_ptr_index], 1, (void*) write_buf);
                    		ind_ptr_index++;
                	}
            	}

            	length_left -= BLOCK_SZ;
        }

        else {
            int w;
            for(w = length - length_left; w < length; w++){
                write_buf = appendCharToCharArray(write_buf, buf[w], len_write_buf);
                len_write_buf++;
                count++;
            }

            if(ptr_index < 12) {
                n->data_ptrs[ptr_index] = get_index();
                write_blocks(n->data_ptrs[ptr_index], 1, (void*) write_buf);
                ptr_index++;

            } else {

                if(n->indirect_ptrs != -1) {
                    indirect_t* indirect_pointer = malloc(sizeof(indirect_t));
                    read_blocks(n->indirect_ptrs, 1, (void*) indirect_pointer);
                    indirect_pointer->data_ptr[ind_ptr_index] = get_index();
                    write_blocks(n->indirect_ptrs, 1, (void*) indirect_pointer);
                    write_blocks(indirect_pointer->data_ptr[ind_ptr_index], 1, (void*) write_buf);
                    ind_ptr_index++;
                }

                else{
                    indirect_t* indirect_pointer = malloc(sizeof(indirect_t));
                    for(ind_ptr_index = 0; ind_ptr_index < NUM_INDIRECT; ind_ptr_index++){
                        indirect_pointer->data_ptr[ind_ptr_index] = -1;
                    }
                    n->indirect_ptrs = get_index();
                    write_blocks(n->indirect_ptrs, 1, (void*) indirect_pointer);
                    ind_ptr_index = 0;

              
                    indirect_pointer->data_ptr[ind_ptr_index] = get_index();
                    write_blocks(n->indirect_ptrs, 1, (void*) indirect_pointer);
                    write_blocks(indirect_pointer->data_ptr[ind_ptr_index], 1, (void*) write_buf);
                    ind_ptr_index++;
                }
            }
        }
    }


    f->rwptr += length;

    // update bitmap
    uint8_t* free_bit_map = get_bitmap();
    write_blocks(NUM_BLOCKS - 1, 1, (void*) free_bit_map);

    // update inode
    n->size += length;
    write_blocks(1, (int) sb.inode_table_len, (void*) inode_table);

    return count;
}

