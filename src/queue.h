#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include "pcb.h"

typedef struct QueueNode {
    PCB* pcb;
    struct QueueNode* next;
} QueueNode;

typedef struct Queue {
    QueueNode* head;
    QueueNode* tail;
    int size;
} Queue;

Queue* queue_create(void);
void queue_destroy(Queue* queue);
void queue_enqueue(Queue* queue, PCB* pcb);
PCB* queue_dequeue(Queue* queue);
PCB* queue_peek(Queue* queue);
bool queue_is_empty(Queue* queue);
int queue_size(Queue* queue);
void queue_remove(Queue* queue, PCB* pcb);
PCB* queue_find_by_pid(Queue* queue, int pid);

#endif // QUEUE_H

