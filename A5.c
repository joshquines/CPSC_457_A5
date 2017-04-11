
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include "queueStruct.c"

typedef struct
{
    prod_cons_queue* queue;
    pthread_mutex_t* lock;
    int id;		// not a pointer			
} producerStruct;


typedef struct
{
    prod_cons_queue* queue;
    pthread_mutex_t* lock;
} consumerStruct;


void* producer(void* arg)
{
    // Get object
	producerStruct* prodStruct = (producerStruct*) arg;

	printf("\tProducer ID: %i\n", prodStruct->id);

    // 10 producers to init 
    int prodCount = 0;
    while (prodCount < 10){

        // Lock tbe tbread
        pthread_mutex_lock(prodStruct->lock); 
        // If queue is full, 
		if(prodStruct->queue->remaining_elements==20)
		{
            printf("QUEUE: FULL\n"); // Debug 
            // Anoher producer is waiting 
            prodStruct->queue->wait++;  
            // Waiting 
            pthread_cond_wait(&prodStruct->queue->pCond, prodStruct->lock);
		}        
        // Add to queue 
        queue_add(prodStruct->queue, prodStruct->id);      
        // Queue added signal to consumerStruct
        pthread_cond_signal(&prodStruct->queue->cCond);
        // Unlock Thread 
        pthread_mutex_unlock(prodStruct->lock);
        prodCount++;
    }
    pthread_exit(0);
}


void* consumer(void* args){
    // Get object 
    consumerStruct* consStruct = (consumerStruct*) args;
    // 100 messages
    int msgCount = 0;

	while(msgCount < 100){
        // Lock Thread 
		pthread_mutex_lock(consStruct->lock);
        
        // If queue empty
        if(consStruct->queue->remaining_elements == 0)
        {
            printf("QUEUE: EMPTY\n"); 
            // Wait
            pthread_cond_wait(&consStruct->queue->cCond, consStruct->lock);
        }
		
        // Remove from queue after waiting 
		int result = queue_remove(consStruct->queue);

         printf("Element Removed: %i\n", result); 
        // If there is a producer waiting 
        if(consStruct->queue->wait > 0)
        {
            // Decrement waiting producers 
            consStruct->queue->wait--;
            // Done signal sent to producer 
            pthread_cond_signal(&consStruct->queue->pCond);
        }
        
        // unlock
        pthread_mutex_unlock(consStruct->lock);
        msgCount++;
    }
    pthread_exit(0);
}


int main()
{
    printf("Program Start\n");
    
    // initialize the lock
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);
	
    
    // initialize threads
    pthread_t threadProd[10];
    producerStruct prodArgs[10];
    pthread_t threadCon;
    
    // initialize the queue
    prod_cons_queue queue;
    queue_initialize(&queue);
    
    // initialize the holder for consumer
    consumerStruct consStruct;
    consStruct.queue = &queue;
    consStruct.lock = &lock;
        
    // loop through and create threads args
	for (int k=0; k<10; k++)
    {
		// initialize producer struct
		prodArgs[k].queue = &queue;
		prodArgs[k].lock = &lock;
		prodArgs[k].id = k + 1;		// passed actual value in
	}
    int rc;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // create 1 consumerStructthread
    rc = pthread_create(&threadCon, &attr, consumer, &consStruct);
    if (rc) {
            printf("ERROR; return code from consumerStructpthread_create() is %d\n", rc);
            exit(-1);
    }

    // create 10 producer threads

    for (int i=0; i<10; i++){
        rc = pthread_create(&threadProd[i], &attr, producer, &prodArgs[i]);
        if (rc) {
            printf("ERROR; return code from producer pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    
    // join threads - waits for threads to terminate
    rc = pthread_join(threadCon, NULL);
    if (rc) {
            printf("ERROR; return code from consumerStructpthread_join() is %d\n", rc);
            exit(-1);
        }
    for (int i=0; i<10; i++){
        rc = pthread_join(threadProd[i], NULL);
        if (rc) {
            printf("ERROR; return code from producer pthread_join() is %d\n", rc);
            exit(-1);
        }
    }

    printf("--------------\nfinal queue: \n");
    for(int i=0; i<20;i++){
        printf("%i: %i\n", i, queue.element[i]);
    }
    return 0;
}
