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
typedef struct{
	int active;
	unsigned long request;
	char request_status;
	unsigned long result[SLOT_COUNT];
	char result_status[SLOT_COUNT];
	char progress[SLOT_COUNT];
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