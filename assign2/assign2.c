/*
 * assign2.c
 *
 * Name: Michelle Song
 * Student Number: V00920619
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include "train.h"
#include <stdbool.h>

pthread_mutex_t	bridgeLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t	mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t	queueLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t	allowedToGo = PTHREAD_COND_INITIALIZER;

volatile int turn = 0;
int		trainCount = 0;

queue eastQ;
queue westQ;

// int eastQueue[1000];
// eastCount = 0;
// int westQueue[1000]; 
// westCount = 0;
/*
 * If you uncomment the following line, some debugging
 * output will be produced.
 *
 * Be sure to comment this line out again before you submit 
 */

 //#define DEBUG	1 

void ArriveBridge (TrainInfo *train);
void CrossBridge (TrainInfo *train);
void LeaveBridge (TrainInfo *train);

/*
 * This function is started for each thread created by the
 * main thread.  Each thread is given a TrainInfo structure
 * that specifies information about the train the individual 
 * thread is supposed to simulate.
 */
void * Train ( void *arguments )
{
	TrainInfo	*train = (TrainInfo *)arguments;

	/* Sleep to simulate different arrival times */
	usleep (train->length*SLEEP_MULTIPLE);

	ArriveBridge (train);
	CrossBridge  (train);
	LeaveBridge  (train); 

	/* I decided that the paramter structure would be malloc'd 
	 * in the main thread, but the individual threads are responsible
	 * for freeing the memory.
	 *
	 * This way I didn't have to keep an array of parameter pointers
	 * in the main thread.
	 */
	free (train);
	return 0;
}

/*
 * You will need to add code to this function to ensure that
 * the trains cross the bridge in the correct order.
 */
void ArriveBridge ( TrainInfo *train ){

	printf ("Train %2d arrives going %s\n", train->trainId, 
			(train->direction == DIRECTION_WEST ? "West" : "East"));

	//add trains to 2 queues
	pthread_mutex_lock(&queueLock);

	if(train->direction == DIRECTION_WEST){
		//printf("train west here\n");
		if(westQ.count == 0){
			train->arrival = 0;
		}
		westQ.head[westQ.count] = train;
		westQ.count++;
	}else if(train->direction == DIRECTION_EAST){
		//printf("train east here\n");
		//printf("%d\n",eastQ.count);
		eastQ.head[eastQ.count] = train;
		eastQ.count++;
	}

	pthread_mutex_unlock(&queueLock);

	pthread_mutex_lock(&bridgeLock);

	train->arrival = train->trainId;

	if(train->direction == DIRECTION_WEST){
		//wait for certain conditions to go
		while(!(westQ.head[0] == train && (eastQ.count == 0 || westQ.crossed == 2))){
			pthread_cond_wait(&allowedToGo, &bridgeLock);
		}
		westQ.crossed = 0;

	}else if(train->direction == DIRECTION_EAST){
		//wait for conditions to go
		while(!(eastQ.head[0] == train && (westQ.count == 0 || westQ.crossed < 2))){
			pthread_cond_wait(&allowedToGo, &bridgeLock);
		}
		//eastQ.crossed++;
		westQ.crossed++;
		
	}

}

/*
 * Simulate crossing the bridge.  You shouldn't have to change this
 * function.
 */
void CrossBridge ( TrainInfo *train )
{
	printf ("Train %2d is ON the bridge (%s)\n", train->trainId,
			(train->direction == DIRECTION_WEST ? "West" : "East"));
	fflush(stdout);
	
	/* 
	 * This sleep statement simulates the time it takes to 
	 * cross the bridge.  Longer trains take more time.
	 */
	usleep (train->length*SLEEP_MULTIPLE);

	printf ("Train %2d is OFF the bridge(%s)\n", train->trainId, 
			(train->direction == DIRECTION_WEST ? "West" : "East"));
	fflush(stdout);
}

/*
 * Add code here to make the bridge available to waiting
 * trains...
 */
void LeaveBridge ( TrainInfo *train ){

	//remove the trains from queue
	if(train->direction == DIRECTION_WEST){
		//shift head
		for(int i = 0; i < westQ.count - 1; i++){
			westQ.head[i] = westQ.head[i+1];
		}
		//westQ.crossed++;
		westQ.count--;

	}else if(train->direction == DIRECTION_EAST){
		//shift head
		for(int i = 0; i < eastQ.count - 1; i++){
			eastQ.head[i] = eastQ.head[i+1];
		}
		//eastQ.crossed++;
		eastQ.count--;
	}

	pthread_cond_broadcast (&allowedToGo);
	pthread_mutex_unlock(&bridgeLock);
	
}

int main ( int argc, char *argv[] )
{

	char 		*filename = NULL;
	pthread_t	*tids;
	int		i;

		
	/* Parse the arguments */
	if ( argc < 2 )
	{
		printf ("Usage: part1 n {filename}\n\t\tn is number of trains\n");
		printf ("\t\tfilename is input file to use (optional)\n");
		exit(0);
	}
	
	if ( argc >= 2 )
	{
		trainCount = atoi(argv[1]);
		//create 2 queues based on size 
		westQ.head = malloc(sizeof(TrainInfo*)*trainCount);
		eastQ.head = malloc(sizeof(TrainInfo*)*trainCount);
	}
	if ( argc == 3 )
	{
		filename = argv[2];
	}	
	
	initTrain(filename);
	
	/*
	 * Since the number of trains to simulate is specified on the command
	 * line, we need to malloc space to store the thread ids of each train
	 * thread.
	 */
	tids = (pthread_t *) malloc(sizeof(pthread_t)*trainCount);
	
	/*
	 * Create all the train threads pass them the information about
	 * length and direction as a TrainInfo structure
	 */
	for (i=0;i<trainCount;i++)
	{
		TrainInfo *info = createTrain();
		
		printf ("Train %2d headed %s length is %d\n", info->trainId,
			(info->direction == DIRECTION_WEST ? "West" : "East"),
			info->length );

		if ( pthread_create (&tids[i],0, Train, (void *)info) != 0 )
		{
			printf ("Failed creation of Train.\n");
			exit(0);
		}
	}


	/*
	 * This code waits for all train threads to terminate
	 */
	for (i=0;i<trainCount;i++)
	{
		pthread_join (tids[i], NULL);
	}
	
	free(tids);
	return 0;
}

