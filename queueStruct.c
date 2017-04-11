
#include <stdio.h>
#include <pthread.h>
#include "queueStruct.h"


void queue_initialize(prod_cons_queue *q)
{
	printf("queue inializing\n"); 
 
    // set element pointers to NULL
	for (int i=0; i<20; i++){		
        q->element[i] = 0; // NULL gives a warning 
	}
    
    // initialize head and tail
	q->head = 0;
	q->tail = 0;
    q->remaining_elements = 0;
    q->wait = 0;

    // initialize conditions
    pthread_cond_init(&q->pCond, NULL);
    pthread_cond_init(&q->cCond, NULL);
}


void queue_add(prod_cons_queue *q, int element)
{
	q->element[q->head] = element;              // make head the element
	q->head = (q->head + 1) % MAX_QUEUE_SIZE;   // increment head, pointer increments circularly
    q->remaining_elements++;                    // increment remaining elements
    if (element < 10){
        printf("Added %i\t\t(%i REMAINING, %i WAITING)\n", element, q->remaining_elements, q->wait);  //Debug
    }else{
        printf("Added %i\t(%i REMAINING, %i WAITING)\n", element, q->remaining_elements, q->wait);  //Debug
    }
}


int queue_remove(prod_cons_queue *q)
{
    int data1 = 0, *ptr , **data;
    
	data1 = q->element[q->tail];        // set data1 to the tail	
	ptr = &data1;                       // pointer to data1
	data = &ptr;                        // double pointer
	
	q->element[q->tail] = 0;            // reset the element
	
	// increment the tail, pointer increments circularly
	q->tail = (q->tail + 1) % MAX_QUEUE_SIZE;
	
    // decrement remaining elements , if its already 0 dont decrement further
	if(q->remaining_elements > 0){
        q->remaining_elements--;
    }

    printf("Removed %i\t(%i REMAINING, %i WAITING)\n", **data, q->remaining_elements, q->wait); //Debug
    
    return **data;          // the removed element is returned in a double 
}
