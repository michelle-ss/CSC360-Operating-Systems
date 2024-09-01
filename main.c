/*
 * main.c
 *
 * Assignment 1 CSC360

 * Michelle Song
 * V00920619
 *
 * Code used from in-class examples: fork_exec.c, 
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>


//global variables
char *command;
int parameters = 0;
pid_t bgList[5];
char* commandsList[5];
int bgCount = 0;
char *ptr;
char bgStatus [5];

char** getInput(){

		//clear global variables
		parameters = 0;

		long size;
		char *buf;
		size = pathconf(".", _PC_PATH_MAX);

		if ((buf = (char *)malloc((size_t)size)) != NULL){
			ptr = getcwd(buf, (size_t)size) ;
		}

		//add ">" symbol
		char* path = malloc(strlen(ptr) + 2);
		strcpy(path, ptr);
		strcat(path, ">");

		//read in input
		char *cmd;
		char *token;
		do{
			cmd = readline (path);

			//tokenize and parse input
			token = strtok(cmd, " ");

		}while(token == NULL);

		char **args = malloc(15 * sizeof(char *));

		while(token != NULL ) {
			//printf( " %s\n", token );
			args[parameters] = token;
			parameters++;
			token = strtok(NULL, " ");
		}

		//add null at end
		args[parameters] = NULL;

		return args;

}

int main ( void ){

	for (;;){

		char ** args = getInput();

		pid_t pid;
		int status;
		//check status of all processes
		for(int i = 0; i < bgCount; i ++){
			pid = waitpid(bgList[i], &status, WNOHANG);
			if(pid > 0){
				printf("Background job with pid %x was terminated\n", bgList[i]);

				//shift bg array once something is deleted
				for(int j = i; i < bgCount-1; i++){
					bgList[j] = bgList[i+1];
					commandsList[j] = commandsList[i+1];
					bgStatus[j] = bgStatus[i+1];
				}
				bgList[bgCount] = 0;
				commandsList[bgCount] = 0;
				bgStatus[bgCount] = 0;
				bgCount--;
			}
		}

		//if cd command input
		if(strcmp(args[0], "cd") == 0){
			chdir(args[1]);

		//if background input	
		}else if(strcmp(args[0], "bg") == 0 || strcmp(args[0], "bglist") == 0 || strcmp(args[0], "bgkill") == 0 || strcmp(args[0], "stop") == 0 || strcmp(args[0], "start") == 0){
			
			if(strcmp(args[0], "stop") == 0){
				int job2 = atoi(args[1]);

				int check = kill(bgList[job2], SIGSTOP);

				waitpid(bgList[job2], &status, WCONTINUED | WNOHANG | WUNTRACED);

				if (check == -1) {
					fprintf(stderr, "Value of errno: %d\n", errno);
				}else if (WIFSTOPPED(status)) {
					printf("already stopped\n");
				}else {
					bgStatus[job2] = 'S';
				}

			}

			if(strcmp(args[0], "start") == 0){
				int job3 = atoi(args[1]);

				waitpid(bgList[job3], &status, WCONTINUED | WNOHANG | WUNTRACED);

				int check = kill(bgList[job3], SIGCONT);

				if (check == -1) {
					fprintf(stderr, "Value of errno: %d\n", errno);
				}else if (WIFCONTINUED(status)) {
					printf("already started\n");
				}else {
					bgStatus[job3] = 'R';
				}

				
			}

			if(strcmp(args[0], "bgkill") == 0){

				//cast string input to int
				int job = atoi(args[1]);

				//printf("job number %d\n", job);
				kill(bgList[job], SIGTERM);

				//shift bg array once something is deleted
				for(int i = job; i < bgCount-1; i++){
					bgList[job] = bgList[job+1];
					bgStatus[job] = bgStatus[job+1];
					commandsList[job] = commandsList[job+1];
				}
				bgList[bgCount] = 0;
				bgStatus[bgCount] = 0;
				commandsList[bgCount] = 0;
				bgCount--;

			}

			if(strcmp(args[0], "bg") == 0){

				pid = fork();

				if (pid < 0) {
					printf("Fork failed.\n");
					return -1;
				}

				if ( pid == 0) {
					// Child is here
					int rc;

					char* command = args[1];
					rc = execvp(command, &args[1]);

					// should never get here
					printf("execvp returned.  Oh noes: %d.\n", rc);


				} else {
					// Parent is here
					
					int status;

					//add new process to bg array
					bgList[bgCount] = pid;
					bgStatus[bgCount] = 'R';

					commandsList[bgCount] = args[1];

					bgCount++;
					
					sleep(1);
					waitpid(pid, &status, WNOHANG);
					
				}

			}//if bg

			if(strcmp(args[0], "bglist") == 0){
				if(bgCount == 0){
					printf("Currently no background processes\n");
				}
				for(int i = 0; i < bgCount; i++){
					printf("%d[%c]: Pid: %x Command: %s\n", i, bgStatus[i], bgList[i], commandsList[i]);
				}

				printf("Total Background jobs: %d\n", bgCount);

			}

		//forground execution
		}else{

			pid = fork();

			// both parent and child execute this
			if (pid < 0) {
				printf("Fork failed.\n");
				return -1;
			}

			if ( pid == 0) {
				// Child is here
				int rc;
				rc = execvp(args[0], args);
				// should never get here
				printf("execvp returned.  Oh noes: %d.\n", rc);

			} else {
				// Parent is here
				int status;

				sleep(1);
				wait(&status);  // We are stuck here until child terminates
			}
		}

		// both processes execute here.
		free(args);
		//free (cmd);
		
	}//for

	return 0;	
}//main