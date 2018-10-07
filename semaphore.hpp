// /////////////////////////////////////////////////////////////////////////////
// Filename:        semmaphore.hpp
// Author:          Christopher Skorka
// Date Created:    25/09/2018
// Description:     Semaphore used to synchronise access to recources
// /////////////////////////////////////////////////////////////////////////////

// MACROS //////////////////////////////////////////////////////////////////////
#ifndef SEMAPHORES_H
#define SEMAPHORES_H

// INCLUDES ////////////////////////////////////////////////////////////////////
#include <pthread.h>

// CLASS PROTOTYPES ////////////////////////////////////////////////////////////

// Semaphore: standard semphore used by JobQueue
class Semaphore{

    // VARIABLES ------------------------------------------
    
	pthread_mutex_t mutex;
	pthread_cond_t condition;
	int value;
    
    public:

    // FUNCTIONS ------------------------------------------

    // -------------------------------------------------------------------------
    // constructor for semaphore used by job queue, initially locked
    // Parameters:	void
    // Returns:		void
    // -------------------------------------------------------------------------
    Semaphore();

    // -------------------------------------------------------------------------
    // waits for semaphore to unlock (non-busy wait)
    // Parameters:	void
    // Returns:		void
    // -------------------------------------------------------------------------
    void wait();

    // -------------------------------------------------------------------------
    // signals that the semaphore is unlocked
    // Parameters:	void
    // Returns:		void
    // -------------------------------------------------------------------------
    void post();

};

// MACROS //////////////////////////////////////////////////////////////////////
#endif

// /////////////////////////////////////////////////////////////////////////////
// END OF FILE
// /////////////////////////////////////////////////////////////////////////////