#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
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



void* producer(void* threadArg){

    // Get object from queue
    producerStruct* prod = (producerStruct*) threadArg;

    printf("\tProducer ID is: %i\n", prod->id);

    // Each producer thread sends 10 messages to the consumer 
    int count = 0;
    while(count < 10){
        // Aquire lock thread  
        pthread_mutex_lock(prod->lock);

        // If the queue is full, wait
        if(prod->queue->remaining_elements == 20){
            //printf("Remaining elements = 20\n");
            printf("queue is full\n");
            // increment queue wait 
            prod->queue->wait++;

            // wait for consumer
            pthread_cond_wait(&prod->queue->pCond, prod->lock);
        }

        // Add message(ID) to queue 
        queue_add(prod->queue, prod->id);

        // Signal to consumer 
        pthread_cond_signal(&prod->queue->cCond);

        count++;
        // Release lock 
        pthread_mutex_unlock(prod->lock);
        
    }
}



/*
C O N S U M E R 
psuedocode from tutorial

Acquire lock 
    Remove an element from queue 
    if(elem == queueSize)
        Pthread_cond_signal(&fullcondition)
    Elem--
Release lock

But with unresolved race condition
*/



void* consumer(void* threadArg){

    printf("consumer thread inializer\n"); 
    // Get object from queue 
    consumerStruct* cons = (consumerStruct*) threadArg;

    // loop 100x
    int consCount = 0;
    while(consCount < 100){

        // lockthread
        pthread_mutex_lock(cons->lock);

        
        // If the queue is empty, wait 
        if(cons->queue->remaining_elements == 0){
            printf("queue empty\n"); 
            pthread_cond_wait(&cons->queue->cCond, cons->lock);
        }

        // Remove from queue 
        int result = queue_remove(cons->queue);

        printf("Item Removed: %i\n", result);        

        // If a producer is waiting, 
        if(cons->queue->wait > 0){
            // decrease waiting producers 
            cons->queue->wait--;

            // signal to producer 
            pthread_cond_signal(&cons->queue->pCond);
        }

        // Increment counter
        consCount++;
        printf("Consumer remove count: %i\n", consCount);

        // Release lock 
        pthread_mutex_unlock(cons->lock);

    }

}


int main(){

    printf("Program Starting\n");
    
    // initialize the lock
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);
	
    // initialize the queue
    prod_cons_queue queue;
    queue_initialize(&queue);

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

    // initialize  and create threads
    int rc; 
    
    pthread_t consThread;
    pthread_t prodThreads[10];

    /* Initialize thread joinable attribute */
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // create 1 consumer thread
    rc = pthread_create(&consThread, &attr, consumer, &consThreadArgs);
    if (rc) {
	        printf("ERROR; return code from consumer pthread_create() is %d\n", rc);
	        exit(-1);
	}

    // create 10 producer threads
    for (int i=0; i<10; i++){
        rc = pthread_create(&prodThreads[i], &attr, producer, &prodThreadArgs[i]);
        if (rc) {
	        printf("ERROR; return code from producer pthread_create() is %d\n", rc);
	        exit(-1);
	    }
    }
    
    // join threads - waits for threads to terminate
    rc = pthread_join(consThread, NULL);
    if (rc) {
	        printf("ERROR; return code from consumer pthread_join() is %d\n", rc);
	        exit(-1);
	    }

    for (int i=0; i<10; i++){
        rc = pthread_join(prodThreads[i], NULL);
        if (rc) {
	        printf("ERROR; return code from producer pthread_join() is %d\n", rc);
	        exit(-1);
	    }
    }    

    printf("\n\tfinal queue: \n");
        for(int i=0; i<20;i++)
        {
            printf("%i: %i\n", i, queue.element[i]);
        }

    pthread_exit(NULL);
}