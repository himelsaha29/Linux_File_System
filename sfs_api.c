#include "sfs_api.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

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


int sfs_getfilesize(const char*);

int sfs_fwrite(int, const char*, int);

int sfs_fread(int, char*, int);

int sfs_remove(char*);
