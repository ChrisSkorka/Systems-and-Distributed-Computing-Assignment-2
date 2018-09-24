// /////////////////////////////////////////////////////////////////////////////
// Filename:        server.cpp
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
#include "threadpool.hpp"

// TYPEDEFS ////////////////////////////////////////////////////////////////////

// GLOBALS /////////////////////////////////////////////////////////////////////
Memory* client;						// shared memory to communicate with client
int threadPoolSize = 320;			// size of the thread pool
unsigned long numbers[SLOT_COUNT] = {0};	// number currently processing
char slotsUsed[SLOT_COUNT] = {0};			// usage state of each slot

// PROTOTYPES //////////////////////////////////////////////////////////////////
int main(int argc, char** argv);
void factorise(Job* job);
void listenForRequests();
Request* generateRequest(unsigned long number, int slot);

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
	client = getSharedMemory();
	initializeSharedMemory(client);

	// initialize thread pool
	initJobQueue();
	initThreadPool(threadPoolSize, &factorise);

	listenForRequests();
}

// -----------------------------------------------------------------------------
// listens for requests and adds them to the job queue
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void listenForRequests(){

	while(client->active){

		// wait for request
		while(client->request_status == 0 && client->active);
		if(!client->active)
			return;

		// find slot, searches backwards, if no slot found slot = -1
		int slot = 9;
		for(;slot >= 0; slot--){
			if(!slotsUsed[slot]){
				break;
			}
		}

		// if run out of slots abort
		if(slot < 0){
			printf("run out of slots\n");
			client->request_status = 2;
			continue;
		}

		// use slot
		slotsUsed[slot] = 1;

		// read number, and return slot
		unsigned long number = client->request;
		client->request = slot;
		client->request_status = 0;

		printf("Request %lu\n", number);

		Request* request = generateRequest(number, slot);

		// jobQueuePush(number, slot);
	}

}

// -----------------------------------------------------------------------------
// generates a request with the 32 jobs that will need to be done
// Parameters:	number: unsighed long				number to be processed
//				slot: int							slot to write results to
// Returns:		void
// -----------------------------------------------------------------------------
Request* generateRequest(unsigned long number, int slot){

	Request* request = (Request*) malloc(sizeof(Request));

	request->jobsDoneCount = 0;

	// for each rotation
	for(int i = 0; i < 32; i++){

		// new job item
		// Job* job = (Job*) malloc(sizeof(Job));
		request->jobs[i].request = request;
		request->jobs[i].number = number;
		request->jobs[i].slot = slot;
		request->jobs[i].progress = 0;

		// add job to request
		// request->jobs[i] = job;

		// rotate number by 1
		unsigned long n = number;
		number = number >> 1 | number << 31;
		number &= 0xFFFFFFFF; 

	}

	// add all jobs onto the job queue
	for(int i = 0; i < 32; i++)
		jobQueuePush(&request->jobs[i]);

	return request;

}

// -----------------------------------------------------------------------------
// factorises the number and reports results to the given slot
// Parameters:	job: Job*				job to be done, contains number and slot
// Returns:		void
// -----------------------------------------------------------------------------
void factorise(Job* job){

	printf("Im doing %lx, in slot %i\n", job->number, job->slot);

}

// /////////////////////////////////////////////////////////////////////////////
// END OF FILE
// /////////////////////////////////////////////////////////////////////////////