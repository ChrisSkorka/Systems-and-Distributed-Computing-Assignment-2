// /////////////////////////////////////////////////////////////////////////////
// Filename:        sharedmemory.c
// Author:          Christopher Skorka
// Date Created:    10/09/2018
// Description:     Creates shared memory that contained a struct with fields
//					for all communication parameters
// /////////////////////////////////////////////////////////////////////////////

// INCLUDES ////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "sharedmemory.hpp"

// GLOBALS /////////////////////////////////////////////////////////////////////

// PROTOTYPES //////////////////////////////////////////////////////////////////
Memory* getSharedMemory();
void initializeSharedMemory(Memory* sharedmem);

// FUNCTIONS ///////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// gets the pointer to shared memory that contains the Memory struct, its used 
// by the client and server to obtain a pointer to the same object
// Parameters:	void
// Returns:		Memory*	  pointer to the shared memory containing struct Memory
// -----------------------------------------------------------------------------
Memory* getSharedMemory(){
	// setup shared memory
    int shmid;
    key_t key = 1235;
	Memory* sharedmem;

	// get id of shared memory
	shmid = shmget(key, sizeof(Memory), IPC_CREAT | 0666);
	
	// failed to obtain shared memory
    if(shmid < 0)
        return NULL;

	// attach to address space
	sharedmem = (Memory*)shmat(shmid, NULL, 0);

	// if failed to attach to address space
	if((void *)sharedmem == (void *) -1)
		return NULL;

	return sharedmem;
}

// -----------------------------------------------------------------------------
// initializes the hsared memory, sets all fields to 0, except for avtive which 
// is set to 1, this is invoked by the server on startup
// Parameters:	Memory* sharedmem: the shared memory to be initalized
// Returns:		void
// -----------------------------------------------------------------------------
void initializeSharedMemory(Memory* sharedmem){

	// initialize 
	sharedmem->active = 1;
	sharedmem->query = 0;
	sharedmem->query_status = QUERY_EMPTY;
	
	for(int i = 0; i < SLOT_COUNT; i++){
		sharedmem->result[i] = 0;
		sharedmem->result_status[i] = RESULT_EMPTY;
		sharedmem->progress[i] = 0;
	}

}

// /////////////////////////////////////////////////////////////////////////////
// END OF FILE
// /////////////////////////////////////////////////////////////////////////////