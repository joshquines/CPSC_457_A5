#include <pthread.h>
#include <stdint.h>

#define MAX_QUEUE_SIZE 20
typedef struct queue{
    int element[MAX_QUEUE_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t remaining_elements; // # of elements in the queue
    // any more variables that you need can be added
} prod_cons_queue;

void queue_initialize( prod_cons_queue *q );
    // initialize all queue variables and set element pointers to
    // NULL
void queue_add( prod_cons_queue *q, int element );

int queue_remove( prod_cons_queue *q );
    // the removed element is returned in a double pointer “data”