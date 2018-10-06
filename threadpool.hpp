// /////////////////////////////////////////////////////////////////////////////
// Filename:        threadpool.hpp
// Author:          Christopher Skorka
// Date Created:    24/09/2018
// Description:     Creates a thread pool, and manages work to be done
// /////////////////////////////////////////////////////////////////////////////

// MACROS //////////////////////////////////////////////////////////////////////
#ifndef THREADPOOL_H
#define THREADPOOL_H

// INCLUDES ////////////////////////////////////////////////////////////////////
#include <pthread.h>
#include "semaphore.hpp"

// TYPEDEFS ////////////////////////////////////////////////////////////////////
typedef enum { 
	BATCHJOB_EMPTY = 0, 
	BATCHJOB_QUERY = 32, 
	BATCHJOB_TEST = 10 
} BatchJobState;

typedef struct Job Job;
typedef struct BatchJob BatchJob;

struct Job {
	unsigned long number;		// the number to be factorised
	char progress;				// the progress this job has made
	BatchJob* batchJob;			// the request this job is part of
	Job* next;					// next job in queue / linked list
};

struct BatchJob {
	int state;
	Job jobs[32];
	int slot;
	pthread_mutex_t resultAccessMutex;
};

// JobQueue: queue (linked list) of jobs, has pointer to fist and last item
typedef struct {
	Job* first;
	Job* last;
	pthread_mutex_t accessMutex;
	Semaphore hasJobs;
} JobQueue;

// Thread: a thread in the tread pool
typedef struct {
	int id;						// id for a thread
	pthread_t pthread;			// a reference to the thread it self
	pthread_attr_t attr;		// attributes of the thread
} Thread;

// PROTOTYPES //////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// starts the specified number of threads, should be called after initializing
// the job queue
// Parameters:	size: int					number of thread to start
//				jobWorker: function*		pointer to function to do each job
// Returns:		void
// -----------------------------------------------------------------------------
void initThreadPool(int size, void (*)(Job*));

// -----------------------------------------------------------------------------
// initializes a thread. alloctes memory, initializes Thread struct, starts and
// dispatches thread to do threadLoop
// Parameters:	id: int						id of this thread
// Returns:		Thread*						pointer to thread struct
// -----------------------------------------------------------------------------
Thread* initThread(int id);

// -----------------------------------------------------------------------------
// thread loop, while running is true it will wait for a job, do it and repeat
// Parameters:	
// Returns:		
// -----------------------------------------------------------------------------
void* threadLoop(void *vargp);

// -----------------------------------------------------------------------------
// initializes the job queue, it is a struct with pointer to first and last
// items, items inbeween are linked lists
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void initJobQueue();

// -----------------------------------------------------------------------------
// adds a job item onto the linked list / queue
// Parameters:	job: Job*					job to be added
// Returns:		void
// -----------------------------------------------------------------------------
void jobQueuePush(Job* job);

// -----------------------------------------------------------------------------
// gets a job item from the queue
// Parameters:	void
// Returns:		Job*						pointer to a job to be done
// -----------------------------------------------------------------------------
Job* jobQueuePull();

// MACROS //////////////////////////////////////////////////////////////////////
#endif

// /////////////////////////////////////////////////////////////////////////////
// END OF FILE
// /////////////////////////////////////////////////////////////////////////////