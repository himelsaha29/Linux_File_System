#include "sfs_api.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MAX_FIZE_NUM 256
#define MAX_FILE_NAME 16


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




}

int sfs_fseek(int fileID, int loc) {
  if (fileID < 0 || loc < 0) {
    fprintf(stderr, "Bad seek argument");
    return -1;
  }


  fdtable[fileID].rwpointer = loc;
  return 0;
}




int sfs_getnextfilename(char*);

int sfs_getfilesize(const char*);

int sfs_fopen(char*);

int sfs_fclose(int);

int sfs_fwrite(int, const char*, int);

int sfs_fread(int, char*, int);

int sfs_remove(char*);
