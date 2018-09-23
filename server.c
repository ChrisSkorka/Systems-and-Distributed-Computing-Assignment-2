// /////////////////////////////////////////////////////////////////////////////
// Filename:        server.c
// Author:          Christopher Skorka
// Date Created:    10/09/2018
// Description:     Server side of the program, it reads numbers from the client
// 					and invokes threads to compute solutions
// /////////////////////////////////////////////////////////////////////////////

// INCLUDES ////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sharedmemory.h"

// TYPEDEFS ////////////////////////////////////////////////////////////////////
void main(int argc, char** argv);

// GLOBALS /////////////////////////////////////////////////////////////////////
Memory* client;
int threadPoolSize = 320;

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
	printf("Server\n");

	// struct shared with client for communication
	Memory* client = getSharedMemory();

	printf("%c %i\n", client->request_status, client->request);
}