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

#include "sharedmemory.h"

// GLOBALS /////////////////////////////////////////////////////////////////////
#define SHMSZ 27
// PROTOTYPES //////////////////////////////////////////////////////////////////
Memory* getSharedMemory();

// FUNCTIONS ///////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// 
// 
// Parameters:	
// Returns:		
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// gets the pointer to shared memory that contains the Memory struct, its used 
// by the client and server to obtain a pointer to the same object
// Parameters:	void
// Returns:		Memory*	  pointer to the shared memory containing struct Memory
// -----------------------------------------------------------------------------
Memory* getSharedMemory(){
	// setup shared memory
    int shmid;
    key_t key = 1234;
	Memory* sharedmem;

    if((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0)
        return NULL;

	if((void *)(sharedmem = shmat(shmid, NULL, 0)) == (void *) -1)
		return NULL;

	return sharedmem;
}