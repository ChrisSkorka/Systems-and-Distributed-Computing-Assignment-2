// /////////////////////////////////////////////////////////////////////////////
// Filename:        client.c
// Author:          Christopher Skorka
// Date Created:    10/09/2018
// Description:     Client side of the program, it queries the user for numbers
// 					and invokes the server of approriate to do so.
// /////////////////////////////////////////////////////////////////////////////

// INCLUDES ////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "sharedmemory.hpp"

// TYPEDEFS ////////////////////////////////////////////////////////////////////
// typedef enum { false, true } bool;

// GLOBALS /////////////////////////////////////////////////////////////////////
Memory* server;
long long int numbers[10] = {-1};

// PROTOTYPES //////////////////////////////////////////////////////////////////
int main(int argc, char** argv);
void printAbove(char* string);
void* updateProgressThreadRunnable(void *vargp);
void* updateResultsThreadRunnable(void* vargp);

// FUNCTIONS ///////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// 
// 
// Parameters:	
// Returns:		
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// main client, manages all client related tasks
// 
// Parameters:	cmd arguments
// Returns:		int: return status
// -----------------------------------------------------------------------------
int main(int argc, char** argv){
	printf("Client\n");

	// struct shared with server for communication
	server = getSharedMemory();

	// buffer for std input
	char cmd_buffer[256] = "";

	// querry for input
	while(strcmp(cmd_buffer, "q") == 0){
		// get next command
		printf("> ");
		scanf("%s", cmd_buffer);
	}

	server->request = 1234;
	server->request_status = 'X';

	printf("Progess\n");

	// start update progress and results thread
	// setup arguments and start
    pthread_t* updateProgressThread = (pthread_t*) calloc(1, sizeof(pthread_t));
    pthread_t* updateResultsThread = (pthread_t*) calloc(1, sizeof(pthread_t));
    pthread_attr_t* updateProgressThreadAttributes = (pthread_attr_t*) calloc(1, sizeof(pthread_attr_t));
    pthread_attr_t* updateResultsThreadAttributes = (pthread_attr_t*) calloc(1, sizeof(pthread_attr_t));
	pthread_create(updateProgressThread, updateProgressThreadAttributes, &updateProgressThreadRunnable, (void *) NULL);
	pthread_create(updateResultsThread, updateResultsThreadAttributes, &updateResultsThreadRunnable, (void *) NULL);

	printf("> ");

	// query for input loop
	while(strcmp(cmd_buffer, "q") != 0){

		// query
		// cmd_buffer = getpass("Press ENTER to input a number");
		// printf("> %s", cmd_buffer);
		scanf("%s", cmd_buffer);

		// move progress line
		// save, move left, move 1 up, clear input, move 1 up, clear progress, write command, restore, write chammand promt
		printf("\e[s\e[1G\e[1A\e[2K\e[1A\e[2K> %s\e[u> ", cmd_buffer);

		// move 

	}

	
	

	// sleep(5);

	printf("\n");

	// wait for thread to finish
	// pthread_join(updateResultsThread, NULL);

}

// -----------------------------------------------------------------------------
// Inserts a string above the current input line
// Parameters:	string: String		string to be inserted above input line
// Returns:		void
// -----------------------------------------------------------------------------
void printAbove(char* string){

	printf("%s\n", string);

}

// -----------------------------------------------------------------------------
// updates the progress of the server tasks, it updates every 500ms if the user
// is not entering a new command
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void* updateProgressThreadRunnable(void *vargp){

	while(1){

		// compute progress string
		char progressStr[256];
		server->request++;
		sprintf(progressStr, "%li", server->request);

		// insert progress above user input
		// save, move 1 up, move to col 1, clear line, string, restore pos
		// printf("\e[s\e[1A\e[1G\e[2K%s\e[u", progressStr);
		printf("\e[s\e[1A\e[1G\e[2K%s\e[u", progressStr);
		fflush(stdout);

		sleep(1);

	}

}

// -----------------------------------------------------------------------------
// 
// 
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void* updateResultsThreadRunnable(void* vargp){

	// infinite loop
	while(true){

		// for each request possible
		for(int i = 0; i < 10; i++){

			// for each requiest currently processesed
			if(numbers[i] >= 0){

				// if new result is found
				if(server->result_status[i] == 1){

					unsigned long result = server->result[i];
					server->result_status[i] = 0;

					char str[256];
					sprintf(str, "Factor for %lli: %li", numbers[i], result);
					printAbove(str);


				// if requiest has finished processing
				}else if(server->result_status[i] == 2){

					char str[256];
					sprintf(str, "Query for %lli complete", numbers[i]);
					printAbove(str);

					server->result_status[i] = 0;
					numbers[i] = -1;
					
				}
			}
		}
	}
}

// /////////////////////////////////////////////////////////////////////////////
// END OF FILE
// /////////////////////////////////////////////////////////////////////////////