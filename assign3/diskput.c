

/*
CSC360 Assignment3
Michelle Song
V00920619
*/

#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "Constants.h"
//#include <netinit/in.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)


int blockCount;
int fatStart;
int fatBlocks;
int rootStart;
int rootBlocks;
int freeBlocks = 0;
int reservedBlocks = 0;
int allocatedBlocks = 0;

int main(int argc, char *argv[]){

    char *addr;
    int fd;
    struct stat sb;
    //char *file_to_copy;

    fd = open(argv[1], O_RDWR);
    if (fd == -1)
        handle_error("open");
    
    if (fstat(fd, &sb) == -1)           /* To obtain file size */
        handle_error("fstat");

    addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    
    if (addr == MAP_FAILED)
        handle_error("mmap");

    if(argv[2]!= NULL){
        //file_to_copy = argv[2];
        //printf("%s\n" ,file_to_copy);
    }else{
        printf("please enter a filename to copy\n");
    }

    printf("File not found.");
    

    //close map
    munmap(addr, sb.st_size);
    close(fd);

    return(0);
}
