// /////////////////////////////////////////////////////////////////////////////
// Filename:        server.cpp
// Author:          Christopher Skorka
// Date Created:    10/09/2018
// Description:     Server side of the program, it reads numbers from the client
// 					and invokes threads to compute solutions
// /////////////////////////////////////////////////////////////////////////////

// MACROS //////////////////////////////////////////////////////////////////////
#ifndef SERVER_H
#define SERVER_H

// FUNCTIONS ///////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// main server 
// Parameters:	cmd arguments
// Returns:		int: return status
// -----------------------------------------------------------------------------
int main(int argc, char** argv);

// -----------------------------------------------------------------------------
// initializes the batchJob array (sets state to empty and init mutes)
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void initializeBatchJobArray();

// -----------------------------------------------------------------------------
// process client communication, processes new batchJobs and updates progresses
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void processClientCommunication();

// -----------------------------------------------------------------------------
// listens for batchJobs and adds them to the job queue
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void checkForQuery();

// -----------------------------------------------------------------------------
// updates the overall progress of each batchJob, if a job has finished, inform
// the client and clear batchJob
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void updateProgress();

// -----------------------------------------------------------------------------
// generates a batchJob with the 32 jobs that will need to be done and adds these
// jobs to the job queue
// Parameters:	batchJob: BatchJob*					batchJob struct to use
//				number: unsighed long				number to be processed
//				slot: int							slot to write results to
// Returns:		void
// -----------------------------------------------------------------------------
void generateBatchJob(BatchJob* batchJob, unsigned long number, int slot);

// -----------------------------------------------------------------------------
// generates a test batchJob with the 10 test jobs that will need to be done and
// adds these jobs to the job queue
// Parameters:	batchJob: BatchJob*					batchJob struct to use
//				slot: int							slot to write results to
// Returns:		void
// -----------------------------------------------------------------------------
void generateTestBatchJob(BatchJob* batchJob, int slot);

// -----------------------------------------------------------------------------
// factorises the number and reports results to the given slot
// Parameters:	job: Job*				job to be done, contains number and slot
// Returns:		void
// -----------------------------------------------------------------------------
void factorise(Job* job);

// -----------------------------------------------------------------------------
// returns a result of a query to the client in the appropriate slot
// Parameters:	batchJob: BatchJob*			The batch hob the result belongs to
//				result:	unsigned long		the result
// Returns:		void
// -----------------------------------------------------------------------------
void returnResult(BatchJob* batchJob, unsigned long result);

// -----------------------------------------------------------------------------
// sleeps for the specified amount of time
// Parameters:	seconds: int seconds			seconds to sleep
//				naneoSeconds: long				additional nano seconds to sleep
// Returns:		void
// -----------------------------------------------------------------------------
void suspend(int seconds, long nanoSeconds);

// -----------------------------------------------------------------------------
// shuts down the server and disables active flag so the client shuts down too
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void shutdown();

// -----------------------------------------------------------------------------
// signal handler for clean shutdown, if shuwdown requested twice, force exit
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void signalHandler(int s);

// MACROS //////////////////////////////////////////////////////////////////////
#endif

// /////////////////////////////////////////////////////////////////////////////
// END OF FILE
// /////////////////////////////////////////////////////////////////////////////