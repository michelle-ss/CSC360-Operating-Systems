
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

    fd = open(argv[1], O_RDWR);
    if (fd == -1)
        handle_error("open");
    
    if (fstat(fd, &sb) == -1)           /* To obtain file size */
        handle_error("fstat");

    addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    
    if (addr == MAP_FAILED)
        handle_error("mmap");


    // Super block information:
    // Block size: 512
    // Block count: 15360
    // FAT starts: 1
    // FAT blocks: 120
    // Root directory start: 121
    // Root directory blocks: 4
    // FAT information:
    // Free Blocks: 15235
    // Reserved Blocks: 121
    // Allocated Blocks: 4

    printf("Super block information:\n");
    printf("Block size: %d\n",htons(addr[BLOCKSIZE_OFFSET]));

    memcpy(&blockCount, addr+BLOCKCOUNT_OFFSET, 4);
    printf("Block count: %d\n", htonl(blockCount));

    memcpy(&fatStart, addr+FATSTART_OFFSET, 4);
    printf("FAT starts: %d\n", htonl(fatStart));


    memcpy(&fatBlocks, addr+FATBLOCKS_OFFSET, 4);
    printf("FAT blocks: %d\n", htonl(fatBlocks));

    memcpy(&rootStart, addr+ROOTDIRSTART_OFFSET, 4);
    printf("Root directory start: %d\n", htonl(rootStart));

    memcpy(&rootBlocks, addr+ROOTDIRBLOCKS_OFFSET, 4);
    printf("Root directory blocks: %d\n", htonl(rootBlocks));

    printf("\nFAT Information:\n"); // bytes for FAT //end = 4294967295

    for(int i = 0; i < htonl(blockCount); i++){
        int fatEntry; 
        memcpy(&fatEntry, addr + (512*htonl(fatStart)) + i*4, 4);

        if(htonl(fatEntry) == 0){
            freeBlocks++;
        }else if(htonl(fatEntry) == 1){
            reservedBlocks++;
        }else{
            allocatedBlocks++;
        }
    }

    printf("Free Blocks: %d\n", freeBlocks);
    printf("Reserved Blocks: %d\n", reservedBlocks);
    printf("Allocated Blocks: %d\n", allocatedBlocks);

    //close map
    munmap(addr, sb.st_size);
    close(fd);

    return(0);
}
