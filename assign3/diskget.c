
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

#define handle_error(msg) \
   do { perror(msg); exit(EXIT_FAILURE); } while (0)

struct Content{
   char status;
   int fileSize;
   char fileName[30];
   char modDate[14];
}Content;

int main(int argc, char *argv[]) {

    // copies a file from the file system to the current directory in Unix

    char *addr;
    int fd;
    struct stat sb;
    char *file_to_copy;
    int startblock;
    int numBlocks;


    fd = open(argv[1], O_RDWR);
    if (fd == -1)
        handle_error("open");

    if (fstat(fd, &sb) == -1)           /* To obtain file size */
        handle_error("fstat");

    addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    
    if (addr == MAP_FAILED)
        handle_error("mmap");

    if(argv[2]!= NULL){
        file_to_copy = argv[2];
        //printf("%s\n" ,file_to_copy);
    }else{
        printf("please enter a filename to copy\n");
    }

    //get root info
    int rootStart;
    memcpy(&rootStart, addr+ROOTDIRSTART_OFFSET, 4);
    int rootBlocks;
    memcpy(&rootBlocks, addr+ROOTDIRBLOCKS_OFFSET, 4);
    int blockCount;
    memcpy(&blockCount, addr+BLOCKCOUNT_OFFSET, 4);
    //printf("Block count: %d\n", htonl(blockCount));
    
    int fileSize;
    int found = 0; 

    //traverse root directory to find starting block
    for(int i = 0; i < htonl(rootBlocks)*512/64; i++){

        //get file size 

        memcpy(&fileSize, addr + (512*htonl(rootStart)) + DIRECTORY_FILE_SIZE_OFFSET + i*64, 4);

        memcpy(&numBlocks, addr + (512*htonl(rootStart)) + DIRECTORY_BLOCK_COUNT_OFFSET + i*64, 4);

        //get file
        char file[30];
        if(htonl(fileSize) > 0){
            for(int j = 0; j < 30; j++){
            file[j] = addr[512*htonl(rootStart)+ DIRECTORY_FILENAME_OFFSET+j + i*64];
            }
        }

        if(strcmp(file, file_to_copy) == 0){
            if(htonl(fileSize) > 0){
                found = 1;
                memcpy(&startblock, addr+(512*htonl(rootStart)) + DIRECTORY_START_BLOCK_OFFSET + i*64, 4);
                //printf("startblock: %d numBlocks: %d\n", htonl(startblock), htonl(numBlocks));
                break;
            }
        }
    }//for

    if(found == 0){
        printf("File not found.\n");
        exit(EXIT_SUCCESS);
    }

    //traverse through blocks to get information
    //create int array of what blocks information is in 
    int fatStart;
    int blocks[htonl(numBlocks)];
    blocks[0] = htonl(startblock);
    memcpy(&fatStart, addr+FATSTART_OFFSET, 4);

    for(int i = 1; i < htonl(numBlocks) ; i++){
        int blockEntry = blocks[i-1];
        int blockValue;
        memcpy(&blockValue, addr+ (512*htonl(fatStart)) + blockEntry*4, 4);
        blocks[i] = htonl(blockValue);
    }

    char file[512];
    FILE *fp;
    fp = fopen(file_to_copy , "w" );
    //fwrite(file , sizeof(char) , sizeof(file) , fp );

    //add info from blocks to file
    for(int i = 0; i < htonl(numBlocks); i++){
        //printf("%d \n", blocks[i]);
        memcpy(file, addr+ (512*blocks[i]), 512);
        fwrite(file, sizeof(char), 512, fp);
    }

    fclose(fp);

    //close map
    munmap(addr, sb.st_size);
    close(fd);
    return 0;
}