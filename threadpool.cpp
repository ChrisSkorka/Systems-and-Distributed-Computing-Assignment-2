// /////////////////////////////////////////////////////////////////////////////
// Filename:        threadpool.cpp
// Author:          Christopher Skorka
// Date Created:    24/09/2018
// Description:     Creates a thread pool, and manages work to be done
// /////////////////////////////////////////////////////////////////////////////

// INCLUDES ////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "threadpool.hpp"

// GLOBALS /////////////////////////////////////////////////////////////////////
int running = true;
JobQueue jobQueue;
void (*jobWorkerFunction)(Job*);
int z = 0;

// PROTOTYPES //////////////////////////////////////////////////////////////////
void initThreadPool(int size, void (*)(Job*));
Thread* initThread(int id);
void* threadLoop(void *vargp);
void initJobQueue();
void jobQueuePush(Job* job);
Job* jobQueuePull();


// FUNCTIONS ///////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// 
// 
// Parameters:	
// Returns:		
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// starts the specified number of threads, should be called after initializing
// the job queue
// Parameters:	size: int					number of thread to start
//				jobWorker: function*		pointer to function to do each job
// Returns:		void
// -----------------------------------------------------------------------------
void initThreadPool(int size, void (*jobWorker)(Job*)){

	// register function to do each job
	jobWorkerFunction = jobWorker;

	// create all the threads
	Thread** threads = (Thread**) malloc(size * sizeof(Thread*));
	if(threads == NULL)
		printf("Failed creating thread pool pointers\n");


	for(int i = 0; i < size; i++){
		threads[i] = initThread(i);
	}

	// for(int i = 0; i < size; i++)
	// 	printf("%i %lu\n", i, (unsigned long) threads[i]);
	// fflush(stdout);
}

// -----------------------------------------------------------------------------
// initializes a thread. alloctes memory, initializes Thread struct, starts and
// dispatches thread to do threadLoop
// Parameters:	id: int						id of this thread
// Returns:		Thread*						pointer to thread struct
// -----------------------------------------------------------------------------
Thread* initThread(int id){

	// allocate thread struct and store 
	Thread* thread = (Thread*) malloc(sizeof(Thread));
	if(thread == NULL)
		printf("Failed creating thread %i\n", id);

	// init struct
	thread->id = id;
	// printf("%i written\n", thread->id);

	// start thread and add data to thread struct 
	pthread_create(&thread->pthread, NULL, &threadLoop, thread);
	pthread_detach(thread->pthread);

	return thread;
}

// -----------------------------------------------------------------------------
// thread loop, while running is true it will wait for a job, do it and repeat
// Parameters:	
// Returns:		
// -----------------------------------------------------------------------------
void* threadLoop(void *vargp){

	Thread* thread = (Thread*)vargp;
	// sleep(1);

	while(running){
		sleep(1);

		Job* job = jobQueuePull();

		if(job == NULL || !running)
			return NULL;
		jobWorkerFunction(job);

	}

}

// -----------------------------------------------------------------------------
// initializes the job queue, it is a struct with pointer to first and last
// items, items inbeween are linked lists
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void initJobQueue(){

	jobQueue.first = NULL;
	jobQueue.last = NULL;
	pthread_mutex_init(&jobQueue.accessMutex, NULL);
	// jobQueue.hasJobs = new Semaphore(LOCKED);

}

// -----------------------------------------------------------------------------
// adds a job item onto the linked list / queue
// Parameters:	job: Job*					job to be added
// Returns:		void
// -----------------------------------------------------------------------------
void jobQueuePush(Job* job){
	
	pthread_mutex_lock(&jobQueue.accessMutex);

	job->next = NULL;

	// add to empty queue
	if(jobQueue.first == NULL){
		
		jobQueue.first = job;
		jobQueue.last = job;

	// add to not empty queue
	} else {

		jobQueue.last->next = job;
		jobQueue.last = job;

	}

	jobQueue.hasJobs.post();

	pthread_mutex_unlock(&jobQueue.accessMutex);

}

// -----------------------------------------------------------------------------
// gets a job item from the queue
// Parameters:	void
// Returns:		Job*						pointer to a job to be done
// -----------------------------------------------------------------------------
Job* jobQueuePull(){
	
	// TODO synchronise!
	// while(jobQueue.first == NULL);
	jobQueue.hasJobs.wait();

	pthread_mutex_lock(&jobQueue.accessMutex);

	Job* job = jobQueue.first;
	jobQueue.first = job->next;

	// if last job in queue, TODO required?
	if(job->next == NULL)
		jobQueue.last = NULL;
	else
		jobQueue.hasJobs.post();
	
	pthread_mutex_unlock(&jobQueue.accessMutex);

	return job;

}


// /////////////////////////////////////////////////////////////////////////////
// END OF FILE
// /////////////////////////////////////////////////////////////////////////////