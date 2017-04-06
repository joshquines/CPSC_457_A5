#include <stdio.h>
#include <pthread.h>
#include <stding.h>

/*
P R O D U C E R
psuedocode from tutorial
*/

// Acquire lock 
    // if (elem < queueSize)
        // Add to queue 

    // else 
        // pthread_cond_wait(&fullcondition, &lock)
        // Add to queue 
    // Elem++ 
// Release lock


/*
C O N S U M E R 
psuedocode from tutorial
*/

// Acquire lock 
    // Remove an element from queue 
    // if(elem == queueSize)
        // Pthread_cond_signal(&fullcondition)
    // Elem--
// Release lock
