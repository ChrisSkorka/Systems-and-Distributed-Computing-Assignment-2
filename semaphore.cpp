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
// 
// 
// Parameters:	
// Returns:		
// -----------------------------------------------------------------------------

Semaphore::Semaphore(){
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condition, NULL);
    value = LOCKED;
}

void Semaphore::wait(){
    pthread_mutex_lock(&mutex);
    while (value != UNLOCKED) {
        pthread_cond_wait(&condition, &mutex);
    }
    value = LOCKED;
    pthread_mutex_unlock(&mutex);
}

void Semaphore::post(){
    pthread_mutex_lock(&mutex);
    value = UNLOCKED;
    pthread_cond_signal(&condition);
    pthread_mutex_unlock(&mutex);
}

// /////////////////////////////////////////////////////////////////////////////
// END OF FILE
// /////////////////////////////////////////////////////////////////////////////