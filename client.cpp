// /////////////////////////////////////////////////////////////////////////////
// Filename:        client.cpp
// Author:          Christopher Skorka
// Date Created:    10/09/2018
// Description:     Client side of the program, it queries the user for numbers
// 					and invokes the server of approriate to do so.
// /////////////////////////////////////////////////////////////////////////////

// INCLUDES ////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include "sharedmemory.hpp"

// TYPEDEFS ////////////////////////////////////////////////////////////////////

// GLOBALS /////////////////////////////////////////////////////////////////////
Memory* server;
char progressStr[512];
int processingUserInput = 0;

unsigned long numbers[SLOT_COUNT] = {0};
char slotsUsed[SLOT_COUNT] = {0};
struct timeval startTime[SLOT_COUNT];


// PROTOTYPES //////////////////////////////////////////////////////////////////
int main(int argc, char** argv);
void printAbove(char* string);
void* updateProgressThreadRunnable(void *vargp);
void* updateResultsThreadRunnable(void* vargp);
void printProgress();
void shutdown();
void suspend(int seconds, long nanoSeconds);
void signalHandler(int signal);

// FUNCTIONS ///////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// main client, manages all client related tasks
// 
// Parameters:	cmd arguments
// Returns:		int: return status
// -----------------------------------------------------------------------------
int main(int argc, char** argv){
	printf("Client\n");
	printf("Enter 32-bit numbers (up to 10 at a time)\n");

	// register signal handler
	signal(SIGINT, signalHandler);

	// struct shared with server for communication
	server = getSharedMemory();

	// buffer for std input
	char cmd_buffer[256] = "";

	// start update progress and results thread
	// setup arguments and start
    pthread_t* updateProgressThread = (pthread_t*) calloc(1, sizeof(pthread_t));
    pthread_t* updateResultsThread = (pthread_t*) calloc(1, sizeof(pthread_t));
    pthread_attr_t* updateProgressThreadAttributes = (pthread_attr_t*) calloc(1, sizeof(pthread_attr_t));
    pthread_attr_t* updateResultsThreadAttributes = (pthread_attr_t*) calloc(1, sizeof(pthread_attr_t));
	pthread_create(updateProgressThread, updateProgressThreadAttributes, &updateProgressThreadRunnable, (void *) NULL);
	pthread_create(updateResultsThread, updateResultsThreadAttributes, &updateResultsThreadRunnable, (void *) NULL);

	// query for input loop
	while(server->active){

		// wait for query
		processingUserInput = 0;
		char c = getchar();
		processingUserInput = 1;

		// query
		printf("\e[1G\e[2K> ");
		scanf("%s", cmd_buffer);
		getchar();

		// if q quit
		if(strcmp(cmd_buffer, "q") == 0)
			break;

		// move progress line
		// save, move left, move 1 up, clear input, move 1 up, clear progress, write command, restore, write chammand promt
		// printf("\e[s\e[1G\e[1A\e[2K\e[1A\e[2K> %s\e[u> ", cmd_buffer);

		// send request

		long long int number = -1;
		long long int max = 1;
		max <<= 32;
		sscanf(cmd_buffer, "%lli", &number);

		// check if the number is in the range
		if(number < 0 || number >= max){
			printAbove("Invalid input");
			continue;
		}

		// send number to server
		server->query = (unsigned long) number;
		server->query_status = QUERY_READY;

		// wait for response
		while(server->query_status == QUERY_READY && server->active);

		// if accepted
		if(server->query_status == QUERY_EMPTY){

			// if test mode
			if(number == 0){

				int slot = (int) server->query;
				numbers[slot + 0] = number;
				numbers[slot + 1] = number;
				numbers[slot + 2] = number;
				slotsUsed[slot + 0] = 1;
				slotsUsed[slot + 1] = 1;
				slotsUsed[slot + 2] = 1;
				gettimeofday(&startTime[slot + 0], NULL);
				gettimeofday(&startTime[slot + 1], NULL);
				gettimeofday(&startTime[slot + 2], NULL);

			// if normal query
			}else{
				int slot = (int) server->query;
				numbers[slot] = number;
				slotsUsed[slot] = 1;
				gettimeofday(&startTime[slot], NULL);

			}

		}

		// if no slots available
		else if(server->query_status == QUERY_OUT_OF_SLOTS){
			server->query_status = QUERY_EMPTY;
			printAbove("No slots available, wait for a query finish");
		}

		// if test mode cannot be run
		else if(server->query_status == QUERY_TEST_NOT_ACCEPTED){
			server->query_status = QUERY_EMPTY;
			printAbove("Wait for existing queries to finish");
		}

	}

	shutdown();

	printf("\n");

}

// -----------------------------------------------------------------------------
// Inserts a string above the current input line
// Parameters:	string: String		string to be inserted above input line
// Returns:		void
// -----------------------------------------------------------------------------
void printAbove(char* string){

	printf("\n\e[1A\e[2K%s\n%s", string, progressStr);
	// fflush(stdout);

}

// -----------------------------------------------------------------------------
// updates the progress of the server tasks, it updates every 500ms if the user
// is not entering a new command
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void* updateProgressThreadRunnable(void *vargp){

	while(server->active){

		// if user inputs currently, dont update
		if(!processingUserInput)
			printProgress();

		suspend(0, 500000000);
	}

}

// -----------------------------------------------------------------------------
// 
// 
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void* updateResultsThreadRunnable(void* vargp){

	// infinite loop
	while(server->active){

		// if user inputs currently, dont update
			if(processingUserInput){
				suspend(0, 500000000);
				continue;
			}

		// for each request possible
		for(int i = 0; i < SLOT_COUNT; i++){

			// for each requiest currently processesed
			if(slotsUsed[i]){

				// if new result is found
				if(server->result_status[i] == 1){

					unsigned long result = server->result[i];
					server->result_status[i] = 0;

					char str[256];
					sprintf(str, "Factor for %lu: %lu", numbers[i], result);
					printAbove(str);


				// if query has finished processing
				}else if(server->result_status[i] == 2){

					struct timeval endTime;
					gettimeofday(&endTime, NULL);
					
					// calculate time taken for server to respond, end_time - start_time
					double timeElapsed = 
						(double)(endTime.tv_usec - 
							startTime[i].tv_usec) / 1000 +
						(double)(endTime.tv_sec - 
							startTime[i].tv_sec) * 1000;

					char str[256];
					sprintf(str, "Query for %lu complete in %fms", numbers[i], timeElapsed);
					printAbove(str);

					server->result_status[i] = 0;
					slotsUsed[i] = 0;
					
				}
			}
		}

		suspend(0, 100000);
	}
}

// -----------------------------------------------------------------------------
// prints the progress of the server tasks
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void printProgress(){

	// compute progress string
	sprintf(progressStr, "Progress: ");

	// for each slot
	for(int i = 0; i < SLOT_COUNT; i++){

		// if slot is used by request/query
		if(slotsUsed[i]){

			// generate progress bar
			char progressBar[] = "__________";
			for(int j = 0; j < (server->progress[i] + 5)  / 10; j++)
				progressBar[j] = '#';

			// concatinate strings
			int end = strlen(progressStr);
			sprintf(&progressStr[end], "Query %lu: %i%% %s, ", numbers[i], (int) server->progress[i], progressBar);
		}
	}
	

	// insert progress above user input
	// save, move 1 up, move to col 1, clear line, string, restore pos
	// printf("\e[s\e[1A\e[1G\e[2K%s\e[u", progressStr);
	printf("\e[1G\e[2K%s (press ENTER to input)", progressStr);
	fflush(stdout);

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
// shuts down the client and disables active flag so the server shuts down too
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void shutdown(){

	printf("Shutting down...\n");

	// signal to the threads and server to shutdown
	server->active = 0;

}

// -----------------------------------------------------------------------------
// signal handler for clean shutdown, if shuwdown requested twice, force exit
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void signalHandler(int s){
	if(s == SIGINT){
		if(server->active == 0)
			exit(1);
		else
			shutdown();
	}
}

// /////////////////////////////////////////////////////////////////////////////
// END OF FILE
// /////////////////////////////////////////////////////////////////////////////