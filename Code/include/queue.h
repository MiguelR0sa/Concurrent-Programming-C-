/**
 *  \brief A simple linked list queue (with some extra array like operation).
 *  
 * \author Miguel Oliveira e Silva - 2017
 */

#ifndef QUEUE_H
#define QUEUE_H

typedef struct _Item_
{
   struct _Item_* next;
   void* elem;
} Item;

typedef struct _Queue_
{
   int alloc;
   Item* head;
   Item* tail;
   int size;
} Queue;

Queue* new_queue(Queue* queue);
Queue* destroy_queue(Queue* queue);
void in_queue(Queue* queue, void* elem);
void* out_queue(Queue* queue);
int empty_queue(Queue* queue);
int size_queue(Queue* queue);
// array-like operations
void* get_queue(Queue* queue, int idx);
void* remove_queue(Queue* queue, int idx);

#endif
