// /////////////////////////////////////////////////////////////////////////////
// Filename:        sharedmemory.h
// Author:          Christopher Skorka
// Date Created:    10/09/2018
// Description:     Creates shared memory that contained a struct with fields
//					for all communication parameters
// /////////////////////////////////////////////////////////////////////////////

// MACROS //////////////////////////////////////////////////////////////////////
#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

// GLOBALS//////////////////////////////////////////////////////////////////////
#define SLOT_COUNT 10

// TYPEDEFS ////////////////////////////////////////////////////////////////////
// QueryStatus: values for Memory.query_status (see Memory)
typedef enum { 
	QUERY_EMPTY, 				// empty, ready for new query
	QUERY_READY, 				// new query
	QUERY_OUT_OF_SLOTS,			// query rejected, out of slots
	QUERY_TEST_NOT_ACCEPTED 	// query rejected, test mode not available
} QueryStatus;

// ResultStatus: values for Memory.result_status[i] (see Memory)
typedef enum { 
	RESULT_EMPTY, 				// empty, ready for results
	RESULT_READY, 				// new result ready
	RESULT_COMPLETE 			// query is complete
} ResultStatus;

// Memory: shared memory for communcication between server and client
typedef struct{
	int active;					// active state of the system
	unsigned long query;		// query
	char query_status;			// query status see QueryStatus
	unsigned long result[SLOT_COUNT];	// results
	char result_status[SLOT_COUNT];		// result status's see ResultStatus
	char progress[SLOT_COUNT];			// query progress
} Memory;

// PROTOTYPES //////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// gets the pointer to shared memory that contains the Memory struct, its used 
// by the client and server to obtain a pointer to the same object
// Parameters:	void
// Returns:		Memory*	  pointer to the shared memory containing struct Memory
// -----------------------------------------------------------------------------
Memory* getSharedMemory();

// -----------------------------------------------------------------------------
// initializes the hsared memory, sets all fields to 0, except for avtive which 
// is set to 1, this is invoked by the server on startup
// Parameters:	Memory* sharedmem: the shared memory to be initalized
// Returns:		void
// -----------------------------------------------------------------------------
void initializeSharedMemory(Memory* sharedmem);

// MACROS //////////////////////////////////////////////////////////////////////
#endif

// /////////////////////////////////////////////////////////////////////////////
// END OF FILE
// /////////////////////////////////////////////////////////////////////////////