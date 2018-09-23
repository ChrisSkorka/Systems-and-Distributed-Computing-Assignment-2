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
#include <queue>
#include <pthread.h>
#include "sharedmemory.hpp"

// TYPEDEFS ////////////////////////////////////////////////////////////////////
// Work: represents work to be done by one thread
typedef struct {
	unsigned long number;		// the number to be factorised
	int index;					// the slot of this task
} Work;

// Thread: a thread in the tread pool
typedef struct {
	pthread_t thread;			// a reference to the thread it self
	pthread_attr_t attr;		// attributes of the thread
	void* runnable;				// the function to be run
	Work work;					// the work to be done
} Thread;

// GLOBALS /////////////////////////////////////////////////////////////////////
Memory* client;					// shared memory to communicate with client
std::queue<Work> WorkQueue;		// work queue, each work task is added to this
std::queue<Thread> ThreadQueue;	// thread queue, threads are drawn and invoked
int threadPoolSize = 320;		// size of the thread pool

// PROTOTYPES //////////////////////////////////////////////////////////////////
int main(int argc, char** argv);

// FUNCTIONS ///////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// 
// 
// Parameters:	
// Returns:		
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// main server 
// 
// Parameters:	cmd arguments
// Returns:		int: return status
// -----------------------------------------------------------------------------
int main(int argc, char** argv){
	printf("Server\n");

	// if given get thread pool size from cmd arguments
	if(argc >= 2){
		int newThreadPoolSize = atoi(argv[1]);
		if(newThreadPoolSize > 0)
			threadPoolSize = newThreadPoolSize;
	}

	printf("Thread pool size: %i\n", threadPoolSize);

	// struct shared with client for communication
	Memory* client = getSharedMemory();
	initializeSharedMemory(client);




	printf("%c %li\n", client->request_status, client->request);
}

// /////////////////////////////////////////////////////////////////////////////
// END OF FILE
// /////////////////////////////////////////////////////////////////////////////