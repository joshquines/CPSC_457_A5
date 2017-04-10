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

    // Get object from queue
    producerStruct* prod = (producerStruct*) threadArg;

    printf("Producer ID is: %i", prod->id);


    // Each producer thread sends 10 messages to the consumer 
    int count = 0;
    while(count < 10){
        // Lock thread  
        pthread_mutex_lock(prod->lock);

        // If there isn't any elements to add, wait 
        if(prod->queue->remaining_elements == 0){
            printf("Remaining elements = 0");

            // increment queue wait 
            prod->queue->wait++;

            // wait 
            pthread_cond_wait(&prod->queue->pCond, prod->lock);
        }

        // Add to queue 
        queue_add(prod->queue, prod->id);

        // Signal to consumer 
        pthread_cond_signal(&prod->queue->cCond);

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

void* consumerInit(void* args){

}


int main(){

    printf("Program Starting\n");
    
    // initialize the lock
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);
	
    // initialize the queue
    prod_cons_queue queue;
    queue_initialize(&queue);

    // initialize threads
    pthread_t prodThreads[10];
    pthread_t consThread;

    // initialize producer struct arguments for each producer
    producerStruct prodThreadArgs[10];
    for (int i=0; i<10; i++){
		prodThreadArgs[i].queue = &queue;
		prodThreadArgs[i].lock = &lock;
		prodThreadArgs[i].id = i + 1;           // Each producer has a unique ID from 1 to 10
	}

    // initialize consumer struct arguments
    consumerStruct consThreadArgs;
    consThreadArgs.queue = &queue;
    consThreadArgs.lock = &lock;



}