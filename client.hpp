// /////////////////////////////////////////////////////////////////////////////
// Filename:        client.hpp
// Author:          Christopher Skorka
// Date Created:    10/09/2018
// Description:     Client side of the program, it queries the user for numbers
// 					and invokes the server of approriate to do so.
// /////////////////////////////////////////////////////////////////////////////

// MACROS //////////////////////////////////////////////////////////////////////
#ifndef CLIENT_H
#define CLIENT_H

// FUNCTIONS ///////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// main client, manages all client related tasks
// Parameters:	cmd arguments
// Returns:		int: return status
// -----------------------------------------------------------------------------
int main(int argc, char** argv);

// -----------------------------------------------------------------------------
// Inserts a string above the current input line
// Parameters:	string: String		string to be inserted above input line
// Returns:		void
// -----------------------------------------------------------------------------
void printAbove(char* string);

// -----------------------------------------------------------------------------
// updates the progress of the server tasks, it updates every 500ms if the user
// is not entering a new command
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void* updateProgressThreadRunnable(void *vargp);

// -----------------------------------------------------------------------------
// reads new results from the server and prints them immediately
// Parameters:	void* vargp:	thread arguents
// Returns:		void
// -----------------------------------------------------------------------------
void* updateResultsThreadRunnable(void* vargp);

// -----------------------------------------------------------------------------
// prints the progress of the server tasks
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void printProgress();

// -----------------------------------------------------------------------------
// sleeps for the specified amount of time
// Parameters:	seconds: int seconds			seconds to sleep
//				naneoSeconds: long				additional nano seconds to sleep
// Returns:		void
// -----------------------------------------------------------------------------
void suspend(int seconds, long nanoSeconds);

// -----------------------------------------------------------------------------
// shuts down the client and disables active flag so the server shuts down too
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