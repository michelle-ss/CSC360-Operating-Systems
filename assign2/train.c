/*
 * train.c
 */
 
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "train.h"
 
/* A global to assign IDs to our trains */ 
int idNumber = 0;

/* If this value is set to 1, trains lengths
 * etc will be generated randomly.
 * 
 * If it is set to 0, the lengths etc will be
 * input from a file.
 */
int doRandom = 0;

/* The file to input train data from */
FILE *inputFile;

/* You can assume that no more than 80 characters
 * will be on any line in the input file
 */
#define MAXLINE		80

void	initTrain ( char *filename )
{
	doRandom = 0;
	
	/* If no filename is specified, generate randomly */
	if ( !filename ){
		doRandom = 1;
		srandom(getpid());
	}
	else{
		inputFile = fopen(filename, "r");
	}
}
 
/*
 * Allocate a new train structure with a new trainId, trainIds are
 * assigned consecutively, starting at 0
 *
 * Either randomly create the train structures or read them from a file
 *
 * This function malloc's space for the TrainInfo structure.  
 * The caller is responsible for freeing it.
 */
TrainInfo *createTrain ( void )
{
	TrainInfo *info = (TrainInfo *)malloc(sizeof(TrainInfo));

	/* I'm assigning the random values here in case
	 * there is a problem with the input file.  Then
	 * at least we know all the fields are initialized.
	 */	 
	info->trainId = idNumber++;
	info->arrival = 0;
	info->direction = (random() % 2 + 1);
	info->length = (random() % MAX_LENGTH) + MIN_LENGTH;

	if (!doRandom){
		/* Your code here to read a line of input
		 * from the input file 
		 */

		 int c;
		 int counter = 1;
		 int firstDigit = 0;
 
		while ( (c = fgetc(inputFile)) != EOF ){
			//printf("%c", c);
			if ( c == '\n' ){
				break;
			}

			info->trainId = idNumber - 1;
			info->arrival = 0;
			if(counter == 1){
				if(c == 'E' || c == 'e') {
					info->direction = DIRECTION_EAST;
				}else{
					info->direction = DIRECTION_WEST;
				}
				counter++;
			}else if(counter == 2){
				firstDigit = c - 48;
				info->length = (c - 48);
				counter++;
			}else if(counter == 3){
				info->length = (c - 48) + firstDigit * 10;
			}
			
		}
		//fclose(inputFile); cannot put here figure out where to close the file
			
	}
	return info;
}


