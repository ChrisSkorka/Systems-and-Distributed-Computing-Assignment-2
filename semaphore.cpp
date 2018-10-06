// /////////////////////////////////////////////////////////////////////////////
// Filename:        semmaphore.cpp
// Author:          Christopher Skorka
// Date Created:    25/09/2018
// Description:     Semaphore used to synchronise access to recources
// /////////////////////////////////////////////////////////////////////////////

// INCLUDES ////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "semaphore.hpp"

// GLOBALS /////////////////////////////////////////////////////////////////////

// PROTOTYPES //////////////////////////////////////////////////////////////////

// FUNCTIONS ///////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// constructor for semaphore used by job queue, initially locked
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
Semaphore::Semaphore(){
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condition, NULL);
    value = LOCKED;
}

// -----------------------------------------------------------------------------
// waits for semaphore to unlock (non-busy wait)
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void Semaphore::wait(){
    pthread_mutex_lock(&mutex);
    while (value != UNLOCKED) {
        pthread_cond_wait(&condition, &mutex);
    }
    value = LOCKED;
    pthread_mutex_unlock(&mutex);
}

// -----------------------------------------------------------------------------
// signals that the semaphore is unlocked
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void Semaphore::post(){
    pthread_mutex_lock(&mutex);
    value = UNLOCKED;
    pthread_cond_signal(&condition);
    pthread_mutex_unlock(&mutex);
}

// /////////////////////////////////////////////////////////////////////////////
// END OF FILE
// /////////////////////////////////////////////////////////////////////////////