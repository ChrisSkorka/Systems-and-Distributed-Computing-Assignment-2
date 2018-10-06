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
#include <signal.h>
#include "sharedmemory.hpp"
#include "threadpool.hpp"

// TYPEDEFS ////////////////////////////////////////////////////////////////////

// GLOBALS /////////////////////////////////////////////////////////////////////
Memory* client;						// shared memory to communicate with client
int threadPoolSize = 320;			// size of the thread pool
BatchJob batchJob[SLOT_COUNT];		// batchJob struct

// PROTOTYPES //////////////////////////////////////////////////////////////////
int main(int argc, char** argv);
void initializeBatchJobArray();
void factorise(Job* job);
void processClientCommunication();
void checkForQuery();
void updateProgress();
void generateBatchJob(BatchJob* batchJob, unsigned long number, int slot);
void generateTestBatchJob(BatchJob* batchJob, int slot);
void returnResult(BatchJob* batchJob, unsigned long result);
void suspend(int seconds, long nanoSeconds);
void shutdown();
void signalHandler(int s);

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

	// init batchJob array
	initializeBatchJobArray();

	// initialize thread pool
	initJobQueue();
	initThreadPool(threadPoolSize, &factorise);

	processClientCommunication();
}

// -----------------------------------------------------------------------------
// initializes the batchJob array (sets state to empty and init mutes)
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void initializeBatchJobArray(){

	for(int i = 0; i < SLOT_COUNT; i++){

		batchJob[i].state = BATCHJOB_EMPTY;
		pthread_mutex_init(&batchJob[i].resultAccessMutex, NULL);

	}
}

// -----------------------------------------------------------------------------
// process client communication, processes new batchJobs and updates progresses
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void processClientCommunication(){

	while(client->active){
		checkForQuery();
		updateProgress();
		suspend(0, 10000000);
	}

	shutdown();

}

// -----------------------------------------------------------------------------
// listens for batchJobs and adds them to the job queue
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void checkForQuery(){

	// wait for batchJob
	if(client->query_status == QUERY_READY){

		// find slot, searches backwards, if no slot found slot = -1
		int slot = 9;
		for(;slot >= 0; slot--){
			if(!batchJob[slot].state){
				break;
			}
		}

		// if run out of slots abort
		if(slot < 0){
			printf("run out of slots\n");
			client->query_status = QUERY_OUT_OF_SLOTS;
			return;
		}

		// read number, and return slot
		unsigned long number = client->query;

		// if test but batchJobs are currently being processed
		if(number == 0){
			for(int i = 0; i < SLOT_COUNT; i++){
				if(batchJob[i].state){
					printf("cannot run test whilst processing queries\n");
					client->query_status = QUERY_TEST_NOT_ACCEPTED;
					return;
				}
			}
		}

		// if test query
		if(number == 0){

			client->query = 0;
			client->query_status = QUERY_EMPTY;

			printf("Test mode\n");

			generateTestBatchJob(&batchJob[0], 0);
			generateTestBatchJob(&batchJob[1], 1);
			generateTestBatchJob(&batchJob[2], 2);

		// if normal query
		}else{

			client->query = slot;
			client->query_status = QUERY_EMPTY;

			printf("Query: %lu\n", number);

			generateBatchJob(&batchJob[slot], number, slot);

		}

	}
}

// -----------------------------------------------------------------------------
// updates the overall progress of each batchJob, if a job has finished, inform
// the client and clear batchJob
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void updateProgress(){

	// for each slot
	for(int i = 0; i < SLOT_COUNT; i++){

		// of batch job struct is currently processing
		if(batchJob[i].state){

			// sum all pgrogress
			int progress = 0;
			for(int j = 0; j < batchJob[i].state; j++)
				progress += batchJob[i].jobs[j].progress;

			// calculate average and send to client
			progress /= batchJob[i].state;
			client->progress[i] = (char) progress;

			// if batchJob is complete, clear batchJob inform client
			if(progress == 100){
				
				// wait for client to read last factor
				while(client->result_status[i]);

				client->result_status[i] = RESULT_COMPLETE;
				batchJob[i].state = BATCHJOB_EMPTY;
			}
		}
	}
}

// -----------------------------------------------------------------------------
// generates a batchJob with the 32 jobs that will need to be done and adds these
// jobs to the job queue
// Parameters:	batchJob: BatchJob*					batchJob struct to use
//				number: unsighed long				number to be processed
//				slot: int							slot to write results to
// Returns:		void
// -----------------------------------------------------------------------------
void generateBatchJob(BatchJob* batchJob, unsigned long number, int slot){

	batchJob->state = BATCHJOB_QUERY;
	batchJob->slot = slot;

	// for each rotation
	for(int i = 0; i < 32; i++){

		// new job item
		// Job* job = (Job*) malloc(sizeof(Job));
		batchJob->jobs[i].batchJob = batchJob;
		batchJob->jobs[i].number = number;
		batchJob->jobs[i].progress = 0;

		// rotate number by 1
		unsigned long n = number;
		number = number >> 1 | number << 31;
		number &= 0xFFFFFFFF; 

	}

	// add all jobs onto the job queue
	for(int i = 0; i < 32; i++)
		jobQueuePush(&batchJob->jobs[i]);

}

// -----------------------------------------------------------------------------
// generates a test batchJob with the 10 test jobs that will need to be done and
// adds these jobs to the job queue
// Parameters:	batchJob: BatchJob*					batchJob struct to use
//				slot: int							slot to write results to
// Returns:		void
// -----------------------------------------------------------------------------
void generateTestBatchJob(BatchJob* batchJob, int slot){

	batchJob->state = BATCHJOB_TEST;
	batchJob->slot = slot;

	// test job
	for(int i = 0; i < 10; i++){

		// new job item
		batchJob->jobs[i].batchJob = batchJob;
		batchJob->jobs[i].number = i * 10;
		batchJob->jobs[i].progress = 0;

	}

	// add all jobs onto the job queue
	for(int i = 0; i < 10; i++)
		jobQueuePush(&batchJob->jobs[i]);

}

// -----------------------------------------------------------------------------
// factorises the number and reports results to the given slot
// Parameters:	job: Job*				job to be done, contains number and slot
// Returns:		void
// -----------------------------------------------------------------------------
void factorise(Job* job){
	printf("A\n");

	// local numbers
	unsigned long number = job->number;
	BatchJob* batchJob = job->batchJob;

	// search space
	unsigned long lower = 1;
	unsigned long upper = number;

	// if test mode change search space
	if(batchJob->state == BATCHJOB_TEST){
		lower = number;
		upper = lower + 9;
	}

	for(unsigned long i = lower; i <= upper; i++){
		printf("B\n");

		// if test mode return number and insert random delay
		if(batchJob->state == BATCHJOB_TEST){
			returnResult(batchJob, i);
			
			// random delay 10ms - 100ms
			suspend(0, 1000000 * (rand() % 90 + 10));

		// if factor found
		}else if(number % i == 0){
			// write factor to slot
			printf("C\n");
			returnResult(batchJob, i);
		}

		// update progress
		job->progress = (char) (100 * (i - lower) / (upper - lower));
	}

}

// -----------------------------------------------------------------------------
// returns a result of a query to the client in the appropriate slot
// Parameters:	batchJob: BatchJob*			The batch hob the result belongs to
//				result:	unsigned long		the result
// Returns:		void
// -----------------------------------------------------------------------------
void returnResult(BatchJob* batchJob, unsigned long result){

	int slot = batchJob->slot;

	// write factor to slot
	pthread_mutex_lock(&batchJob->resultAccessMutex); 

	while(client->result_status[slot]);
	client->result[slot] = result;
	client->result_status[slot] = RESULT_READY;

	pthread_mutex_unlock(&batchJob->resultAccessMutex); 

}

// -----------------------------------------------------------------------------
// sleeps for the specified amount of time
// Parameters:	seconds: int seconds			seconds to sleep
//				naneoSeconds: long				additional nano seconds to sleep
// Returns:		void
// -----------------------------------------------------------------------------
void suspend(int seconds, long nanoSeconds){
    struct timespec ts;
    ts.tv_sec = seconds;
    ts.tv_nsec = nanoSeconds;
	nanosleep(&ts, NULL);
}

// -----------------------------------------------------------------------------
// shuts down the server and disables active flag so the client shuts down too
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void shutdown(){

	printf("Shutting down...\n");

	// signal to the threads and server to shutdown
	client->active = 0;

}

// -----------------------------------------------------------------------------
// signal handler for clean shutdown, if shuwdown requested twice, force exit
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void signalHandler(int s){
	if(s == SIGINT){
		if(client->active == 0)
			exit(1);
		else
			shutdown();
	}
}

// /////////////////////////////////////////////////////////////////////////////
// END OF FILE
// /////////////////////////////////////////////////////////////////////////////