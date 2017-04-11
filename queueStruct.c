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
    q->tail = (q->tail + 1) % MAX_QUEUE_SIZE;                      // increment tail index
    q->remaining_elements++;            // increment remaining elements, one more element added to the queue
    printf("added %i\t(%i REMAINING, %i WAITING)\n", element, q->remaining_elements, q->wait);
}

int queue_remove( prod_cons_queue *q ){
    int **data;

    **data = q->element[q->head];       
    q->element[q->head] = 0;
    q->head = (q->head + 1) % MAX_QUEUE_SIZE;                          //decrement tail index
    if(q->remaining_elements > 0){
        q->remaining_elements--;            // decrement remaining elements, one element removes from the queue
    }
    printf("removed %i\t(%i REMAINING, %i WAITING)\n", **data, q->remaining_elements, q->wait);
    return **data;                      // the removed element is returned in a double pointer “data”
}
    