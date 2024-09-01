
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
   // display contents of root directory

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

   
   //get root info
   int rootStart;
   memcpy(&rootStart, addr+ROOTDIRSTART_OFFSET, 4);
   //printf("Root directory start: %d\n", htonl(rootStart));
   int rootBlocks;
   memcpy(&rootBlocks, addr+ROOTDIRBLOCKS_OFFSET, 4);
   //printf("Root directory blocks: %d\n", htonl(rootBlocks));

   struct Content contents;

   for(int i = 0; i < htonl(rootBlocks)*512/64; i++){

      
      //get status
      if(htons(addr[512*htonl(rootStart) + i*64]) == 768){
         contents.status = 'F';
         //printf("%c\n", contents.status);
         //printf("status value %d\n",htons(addr[512*htonl(rootStart) + i*64]));
      }else if(htons(addr[512*htonl(rootStart) + i*64]) == 1280 || htons(addr[512*htonl(rootStart) + i*64]) == 1024){
         contents.status = 'D';
         //printf("%c", contents.status);
      }

      //get file size 
      int fileSize; 
      memcpy(&fileSize, addr + (512*htonl(rootStart)) + DIRECTORY_FILE_SIZE_OFFSET + i*64, 4);
      if(htonl(fileSize) > 0){
         contents.fileSize = htonl(fileSize);
      }

      //get file
      char file[30];
      if(htonl(fileSize) > 0){
         for(int j = 0; j < 30; j++){
            file[j] = addr[512*htonl(rootStart)+ DIRECTORY_FILENAME_OFFSET+j + i*64];
         }
      }

      //get modify time
      //YYYYMMDDHHMMSS
      int year;
      int month;
      int day;
      int hour;
      int minute;
      int second; 
      memcpy(&year, addr + (512*htonl(rootStart)) + DIRECTORY_MODIFY_OFFSET + i*64, 2);
      
      unsigned char *modifyTime_bytes = (unsigned char *)malloc(sizeof(unsigned char) * 7);
      modifyTime_bytes = memcpy(modifyTime_bytes, addr+(512*htonl(rootStart)) + DIRECTORY_MODIFY_OFFSET + i*64, 7);
      month = modifyTime_bytes[2];
      day = modifyTime_bytes[3];
      hour = modifyTime_bytes[4];
      minute = modifyTime_bytes[5];
      second = modifyTime_bytes[6];

      if(htonl(fileSize) > 0){
         printf("%c %10d %30s %d/%02d/%02d %02d:%02d:%02d\n", contents.status, contents.fileSize, file, ntohs(year), month, day, hour, minute, second);
      }

   }


   //close map
   munmap(addr, sb.st_size);
   close(fd);
   return 0;
}