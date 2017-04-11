
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include "queueStruct.c"

#define PRODUCERS_COUNT 10
#define PRODUCER_ITERATIONS 10

typedef struct
{
    prod_cons_queue* queue;
    pthread_mutex_t* locker;
    int id;		// not a pointer			
} producerStruct;


typedef struct
{
    prod_cons_queue* queue;
    pthread_mutex_t* locker;
} consumerStruct;


void* producerInit(void* arg)
{
    // grab queue holder from args
	producerStruct* prodStruct = (producerStruct*) arg;

	/* DEBUG */
	printf("\tID passed in: %i\n", prodStruct->id);

    // loop through 10 times
    for (int i=0; i<10; i++)
    { 
        // lock
        pthread_mutex_lock(prodStruct->locker);
            
        // wait if no elements to add
		if(prodStruct->queue->remaining_elements==20)
		{
            /* DEBUG */
            printf("queue full\n");
            
            // increase number of waiting producers
            prodStruct->queue->wait++;
            
            // wait
            pthread_cond_wait(&prodStruct->queue->cond1, prodStruct->locker);
		}
        
        // add element
        queue_add(prodStruct->queue, prodStruct->id);
        
        // signal to the consumer it's done
        pthread_cond_signal(&prodStruct->queue->cond2);
       
        /* DEBUG */
        if(DEBUG==0)
        {
            for(int i=0; i<20;i++)
            {
                printf("%i: %i\n", i, prodStruct->queue->element[i]);
            }
        }
        // unlock
        pthread_mutex_unlock(prodStruct->locker);
    }
}


void* consumerInit(void* args)
{
    printf("consumer thread inializer\n"); 
    
    // grab consStruct
    consumerStruct* consStruct = (consumerStruct*) args;
    
    // loop 100 times
	for(int i=0; i<100; i++)
	{
        // lock
		pthread_mutex_lock(consStruct->locker);
        
        // wait if the queue is empty (nothing to consume)
        if(consStruct->queue->remaining_elements == 0)
        {
            printf("queue empty\n"); 
            
            // wait
            pthread_cond_wait(&consStruct->queue->cond2, consStruct->locker);
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
            pthread_cond_signal(&consStruct->queue->cond1);
        }
        
        // unlock
        pthread_mutex_unlock(consStruct->locker);
    }
}


int main()
{
    printf("Program Start\n");
    
    // initialize the lock
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
	
    // initialize the condition variables
    pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
    pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
    
    // initialize threads
    pthread_t prodThreads[10];
    producerStruct prodThreadArgs[10];
    pthread_t consThread;
    
    // initialize the queue
    prod_cons_queue queue;
    queue_initialize(&queue);
    
    // initialize the holder for consumer
    consumerStruct consStruct;
    consStruct.queue = &queue;
    consStruct.locker = &lock;
        
    // loop through and create threads args
	for (int k=0; k<10; k++)
    {
		// initialize producer struct
		prodThreadArgs[k].queue = &queue;
		prodThreadArgs[k].locker = &lock;
		prodThreadArgs[k].id = k + 1;		// passed actual value in
	}
    int rc;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // create 1 consumer thread
    rc = pthread_create(&consThread, &attr, consumerInit, &consStruct);
    if (rc) {
            printf("ERROR; return code from consumer pthread_create() is %d\n", rc);
            exit(-1);
    }

    // create 10 producer threads
    for (int i=0; i<10; i++){
        rc = pthread_create(&prodThreads[i], &attr, producerInit, &prodThreadArgs[i]);
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
