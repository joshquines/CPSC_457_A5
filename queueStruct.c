#include <stdio.h>
#include <pthread.h>
#include "queueStruct.h"


void queue_initialize( prod_cons_queue *q ){

     // set element pointers to NULL
	for (int i=0; i<20; i++){		
        q->element[i] = NULL;
	}
    
    // initialize all queue variables
	q->head = 0;
	q->tail = 0;
	q->remaining_elements=0;
	q->wait=0;

    // initialize condition variables
	pthread_cond_init(&q->pCond, NULL); 
    pthread_cond_init(&q->cCond, NULL); 
}
    
void queue_add( prod_cons_queue *q, int element ){
    q->element[q->tail] = element;      // add element to the end of queue
    q->tail++;                          // increment tail index
    q->remaining_elements++;            // increment remaining elements, one more element added to the queue

}

int queue_remove( prod_cons_queue *q ){
    int **data;

    **data = q->element[q->tail];       
    q->tail--;                          //decrement tail index
    q->remaining_elements--;            // decrement remaining elements, one element removes from the queue

    return **data;                      // the removed element is returned in a double pointer “data”
}
    