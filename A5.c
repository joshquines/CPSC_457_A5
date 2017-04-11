
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


void* initProd(void* arg)
{
    // grab queue holder from args
	producerStruct* prodStruct = (producerStruct*) arg;

	/* DEBUG */
	printf("\tProducer ID: %i\n", prodStruct->id);

    // 10 producers to init 
    int prodCount = 0;
    while (prodCount < 10){

        // Lock tbe tbread
        pthread_mutex_lock(prodStruct->lock);
            
        // If queue is full, 
		if(prodStruct->queue->remaining_elements==20)
		{
            printf("QUEUE: FULL\n");
            
            // Anoher producer is waiting 
            prodStruct->queue->wait++;
            
            // Waiting 
            pthread_cond_wait(&prodStruct->queue->pCond, prodStruct->lock);
		}
        
        // Add to queue 
        queue_add(prodStruct->queue, prodStruct->id);
        
        // Queue added signal to consumer 
        pthread_cond_signal(&prodStruct->queue->cCond);
       

        // Unlock Thread 
        pthread_mutex_unlock(prodStruct->lock);
        prodCount++;
    }
}


void* initCons(void* args)
{
    printf("consumer thread inializer\n"); 
    
    // grab consStruct
    consumerStruct* consStruct = (consumerStruct*) args;
    
    // loop 100 times
	for(int i=0; i<100; i++)
	{
        // lock
		pthread_mutex_lock(consStruct->lock);
        
        // wait if the queue is empty (nothing to consume)
        if(consStruct->queue->remaining_elements == 0)
        {
            printf("queue empty\n"); 
            
            // wait
            pthread_cond_wait(&consStruct->queue->cCond, consStruct->lock);
        }
		
        // remove from queue
		int result = queue_remove(consStruct->queue);

         printf("Item Removed: %i\n", result); 
        
        // check if someone is waiting
        if(consStruct->queue->wait > 0)
        {
            // decrease number of waiting producers
            consStruct->queue->wait--;
            
            // signal to producer that it is done
            pthread_cond_signal(&consStruct->queue->pCond);
        }
        
        // unlock
        pthread_mutex_unlock(consStruct->lock);
    }
}


int main()
{
    printf("Program Start\n");
    
    // initialize the lock
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
	
    // initialize the condition variables
    pthread_cond_t pCond = PTHREAD_COND_INITIALIZER;
    pthread_cond_t cCond = PTHREAD_COND_INITIALIZER;
    
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

    // create 1 consumer thread
    rc = pthread_create(&threadCon, &attr, initCons, &consStruct);
    if (rc) {
            printf("ERROR; return code from consumer pthread_create() is %d\n", rc);
            exit(-1);
    }

    // create 10 producer threads

    for (int i=0; i<10; i++){
        rc = pthread_create(&threadProd[i], &attr, initProd, &prodArgs[i]);
        if (rc) {
            printf("ERROR; return code from producer pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    
    // join threads - waits for threads to terminate
    rc = pthread_join(threadCon, NULL);
    if (rc) {
            printf("ERROR; return code from consumer pthread_join() is %d\n", rc);
            exit(-1);
        }
    for (int i=0; i<10; i++){
        rc = pthread_join(threadProd[i], NULL);
        if (rc) {
            printf("ERROR; return code from producer pthread_join() is %d\n", rc);
            exit(-1);
        }
    }

	/* DEBUG */
    if(DEBUG==1)
	{
        printf("--------------\nfinal queue: \n");
        for(int i=0; i<20;i++)
        {
            printf("%i: %i\n", i, queue.element[i]);
        }
    }
    
    return 0;
}
