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
#include <string.h>
#include "sharedmemory.h"
#include <pthread.h>

// TYPEDEFS ////////////////////////////////////////////////////////////////////
typedef enum { false, true } bool;

// GLOBALS /////////////////////////////////////////////////////////////////////
// PROTOTYPES //////////////////////////////////////////////////////////////////
void main(int argc, char** argv);
void updateProgress(void *vargp);

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
// Returns:		void
// -----------------------------------------------------------------------------
void main(int argc, char** argv){
	printf("Client\n");

	// struct shared with server for communication
	Memory* server = getSharedMemory();

	// start progress thread
	// setup arguments and start
    pthread_t* thread = (pthread_t*) calloc(1, sizeof(pthread_t));
    pthread_attr_t* attr = (pthread_attr_t*) calloc(1, sizeof(pthread_attr_t));
	pthread_create(thread, attr, &updateProgress, (void *) server);

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

	sleep(10);

	// wait for thread to finish
	pthread_join(*thread, NULL);

}

// -----------------------------------------------------------------------------
// updates the progress of the server tasks, it updates every 500ms if the user
// is not entering a new command
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void updateProgress(void *vargp){

	while(1){

		// get shared memory
		Memory* server = (Memory*) vargp;

		// compute progress string
		char* progressStr[256];
		server->request++;
		sprintf(progressStr, "%i", server->request);

		// insert progress above user input
		printf(progressStr);

		sleep(1);

	}

}