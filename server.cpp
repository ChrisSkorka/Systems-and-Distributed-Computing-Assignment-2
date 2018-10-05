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
Request* requests[SLOT_COUNT] = {NULL};		// request struct

// PROTOTYPES //////////////////////////////////////////////////////////////////
int main(int argc, char** argv);
void factorise(Job* job);
void processClientCommunication();
void checkRequests();
void updateProgress();
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

	processClientCommunication();
}

// -----------------------------------------------------------------------------
// process client communication, processes new requests and updates progresses
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void processClientCommunication(){

	while(client->active){
		checkRequests();
		updateProgress();
	}

}

// -----------------------------------------------------------------------------
// listens for requests and adds them to the job queue
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void checkRequests(){

	// wait for request
	if(client->request_status == 1 && client->active){

		// find slot, searches backwards, if no slot found slot = -1
		int slot = 9;
		for(;slot >= 0; slot--){
			if(!requests[slot]){
				break;
			}
		}

		// if run out of slots abort
		if(slot < 0){
			printf("run out of slots\n");
			client->request_status = 2;
			return;
		}

		// read number, and return slot
		unsigned long number = client->request;
		client->request = slot;
		client->request_status = 0;

		printf("Request %lu\n", number);

		requests[slot] = generateRequest(number, slot);

	}
}

// -----------------------------------------------------------------------------
// updates the overall progress of each request, if a job has finished, inform
// the client and clear request
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void updateProgress(){

	for(int i = 0; i < SLOT_COUNT; i++){
		if(requests[i]){

			// sum all pgrogress
			int progress = 0;
			for(int j = 0; j < 32; j++)
				progress += requests[i]->jobs[j].progress;

			// calculate average and send to client
			progress /= 32;
			client->progress[i] = (char) progress;

			// if request is complete, clear request inform client
			if(progress == 100){
				
				// wait for client to read last factor
				while(client->result_status[i]);

				client->result_status[i] = 2;
				requests[i] = NULL;
			}
		}
	}
}

// -----------------------------------------------------------------------------
// generates a request with the 32 jobs that will need to be done and adds these
// jobs to the job queue
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

	// local numbers
	unsigned long number = job->number;
	int slot = job->slot;
	Request* request = job->request;

	for(unsigned long i = 1; i <= number; i++){

		// if factor found
		if(number % i == 0){

			// write factor to slot
			pthread_mutex_lock(&request->resultAccessMutex); 

			while(client->result_status[slot]);
			client->result[slot] = i;
			client->result_status[slot] = 1;

			pthread_mutex_unlock(&request->resultAccessMutex); 

		}

		// update progress
		job->progress = (char) (100 * i / number);
	}

}

// /////////////////////////////////////////////////////////////////////////////
// END OF FILE
// /////////////////////////////////////////////////////////////////////////////