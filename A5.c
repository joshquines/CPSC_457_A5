
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
	producerStruct* prod = (producerStruct*) arg;

	printf("\tProducer ID: %i\n", prod->id);

    // 10 producers to init 
    int prodCount = 0;
    while (prodCount < 10){

        // Lock tbe tbread
        pthread_mutex_lock(prod->lock); 
        // If queue is full, 
		if(prod->queue->remaining_elements==20)
		{
            printf("QUEUE: FULL\n"); // Debug 
            // Anoher producer is waiting 
            prod->queue->wait++;  
            // Waiting 
            pthread_cond_wait(&prod->queue->pCond, prod->lock);
		}        
        // Add to queue 
        queue_add(prod->queue, prod->id);      
        // Queue added signal to consumerStruct
        pthread_cond_signal(&prod->queue->cCond);
        // Unlock Thread 
        pthread_mutex_unlock(prod->lock);
        prodCount++;
    }
    pthread_exit(0);
}


void* consumer(void* args){
    // Get object 
    consumerStruct* cons = (consumerStruct*) args;
    // 100 messages
    int msgCount = 0;

	while(msgCount < 100){
        // Lock Thread 
		pthread_mutex_lock(cons->lock);
        
        // If queue empty
        if(cons->queue->remaining_elements == 0)
        {
            printf("QUEUE: EMPTY\n"); 
            // Wait
            pthread_cond_wait(&cons->queue->cCond, cons->lock);
        }
		
        // Remove from queue after waiting 
		int result = queue_remove(cons->queue);

        //printf("Element Removed: %i\n", result); 
        // If there is a producer waiting 
        if(cons->queue->wait > 0)
        {
            // Decrement waiting producers 
            cons->queue->wait--;
            // Done signal sent to producer 
            pthread_cond_signal(&cons->queue->pCond);
        }
        
        // unlock
        pthread_mutex_unlock(cons->lock);
        msgCount++;
    }
    printf("Consumer done");
    pthread_exit(0);
}


int main(){
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
    consumerStruct cons;
    cons.queue = &queue;
    cons.lock = &lock;
        
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

    // create 1 consumerStructThread
    rc = pthread_create(&threadCon, &attr, consumer, &cons);
    if (rc) {
            printf("ERROR; return code from consumerStructpthread_create() is %d\n", rc);
            exit(-1);
    }

    // 10 Producer Threads 
    int prodThreadCount = 0;
    while(prodThreadCount < 10){

        rc = pthread_create(&threadProd[prodThreadCount], &attr, producer, &prodArgs[prodThreadCount]);
        if (rc) {
            printf("ERROR CODE pthread_create(): %d\n", rc);
            exit(-1);
        }
        prodThreadCount++;
    }
    
    // join threads - waits for threads to terminate
    rc = pthread_join(threadCon, NULL);
    if (rc) {
            printf("ERROR CODE consumerStructpthread_join():  %d\n", rc);
            exit(-1);
        }

    for (int i=0; i<10; i++){
        rc = pthread_join(threadProd[i], NULL);
        if (rc) {
            printf("ERROR; return code from producer pthread_join() is %d\n", rc);
            exit(-1);
        }
    }

    /*printf("--------------\nfinal queue: \n");
    for(int i=0; i<20;i++){
        printf("%i: %i\n", i, queue.element[i]);
    }*/

    //pthread_exit(NULL);
    return 0;
}
