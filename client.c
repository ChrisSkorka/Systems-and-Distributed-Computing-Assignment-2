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

// TYPEDEFS ////////////////////////////////////////////////////////////////////
typedef enum { false, true } bool;

// GLOBALS /////////////////////////////////////////////////////////////////////
// PROTOTYPES //////////////////////////////////////////////////////////////////
// FUNCTIONS ///////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// 
// 
// Parameters:	
// Returns:		
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// 
// 
// Parameters:	
// Returns:		
// -----------------------------------------------------------------------------
void main(int argc, char** argv){
	printf("Client\n");

	// struct shared with server for communication
	Memory* server = getSharedMemory();

	char cmd_buffer[256] = "";

	while(strcmp(cmd_buffer, "q") == 0){
		// get next command
		printf("> ");
		scanf("%s", cmd_buffer);
	}

	server->request = 1234;
	server->request_status = 'X';

	sleep(10);

}