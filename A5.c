#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include "queueStruct.c"


// Setup Producer
typedef struct{
    prod_cons_queue* queue;
    pthread_mutex_t* lock;
    int id;
} producerStruct;

// Setup Consumer
typedef struct{
    prod_cons_queue* queue;
    pthread_mutex_t* lock;
} consumerStruct;

/*
P R O D U C E R
psuedocode from tutorial

Acquire lock 
    if (elem < queueSize)
        Add to queue 

    else 
        pthread_cond_wait(&fullcondition, &lock)
        Add to queue 
    Elem++ 
Release lock

With unresolved race condition
*/



void producer(void* threadArg){

    // 10 Producer threads
    int count = 0;
    while(count < 10){
        // Get object from queue
        producerStruct* prod = (producerStruct*) arg;

        printf("Producer ID is: %i", prod->id);

        // Lock thread  
        pthread_mutex-lock(prod->lock);

        // If there isn't any elements to add, wait 
        if(prod->queue->remaining_elements == 0){
            printf("Remaining elements = 0");

            // increment queue wait 
            prod->queue->wait++;

            // wait  -- what's cond1?
            pthread_cond_wait(&prod->queue->cond)1, prod->lock);
        }

        // Add to queue 
        queue_add(prod->queue, prod->id);

        // Signal to consumer - what's cond2? 
        pthread_cond_signal(&prod->queue->cond2);

        // Release lock 
        pthread_mutex_unlock(prod->lock);
        count++;
    }
}



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
