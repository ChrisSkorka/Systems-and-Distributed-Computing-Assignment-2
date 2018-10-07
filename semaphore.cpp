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

// FUNCTIONS ///////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// constructor for semaphore used by job queue, initially locked
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
Semaphore::Semaphore(){

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condition, NULL);
    value = 0;

}

// -----------------------------------------------------------------------------
// waits for semaphore to unlock (non-busy wait)
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void Semaphore::wait(){

    pthread_mutex_lock(&mutex);
    
    // if no job available wait for signal
    while(value == 0) {
        pthread_cond_wait(&condition, &mutex);
    }
    value -= 1;

    pthread_mutex_unlock(&mutex);
}

// -----------------------------------------------------------------------------
// signals that the semaphore is unlocked
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void Semaphore::post(){
    pthread_mutex_lock(&mutex);

    value = value + 1;

    // signal waiting thread
    pthread_cond_signal(&condition);

    pthread_mutex_unlock(&mutex);

}

// /////////////////////////////////////////////////////////////////////////////
// END OF FILE
// /////////////////////////////////////////////////////////////////////////////