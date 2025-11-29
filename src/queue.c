#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

Queue* queue_create(void) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if (!queue) {
        fprintf(stderr, "Erreur: Impossible d'allouer la file\n");
        return NULL;
    }
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    return queue;
}

void queue_destroy(Queue* queue) {
    if (!queue) return;
    
    QueueNode* current = queue->head;
    while (current) {
        QueueNode* next = current->next;
        free(current);
        current = next;
    }
    free(queue);
}

void queue_enqueue(Queue* queue, PCB* pcb) {
    if (!queue || !pcb) return;
    
    QueueNode* node = (QueueNode*)malloc(sizeof(QueueNode));
    if (!node) {
        fprintf(stderr, "Erreur: Impossible d'allouer le nœud de file\n");
        return;
    }
    
    node->pcb = pcb;
    node->next = NULL;
    
    if (queue->tail) {
        queue->tail->next = node;
    } else {
        queue->head = node;
    }
    queue->tail = node;
    queue->size++;
}

PCB* queue_dequeue(Queue* queue) {
    if (!queue || !queue->head) return NULL;
    
    QueueNode* node = queue->head;
    PCB* pcb = node->pcb;
    
    queue->head = node->next;
    if (!queue->head) {
        queue->tail = NULL;
    }
    
    free(node);
    queue->size--;
    return pcb;
}

PCB* queue_peek(Queue* queue) {
    if (!queue || !queue->head) return NULL;
    return queue->head->pcb;
}

bool queue_is_empty(Queue* queue) {
    return !queue || queue->size == 0;
}

int queue_size(Queue* queue) {
    return queue ? queue->size : 0;
}

void queue_remove(Queue* queue, PCB* pcb) {
    if (!queue || !pcb || !queue->head) return;
    
    QueueNode* current = queue->head;
    QueueNode* prev = NULL;
    
    while (current) {
        if (current->pcb == pcb) {
            if (prev) {
                prev->next = current->next;
            } else {
                queue->head = current->next;
            }
            
            if (current == queue->tail) {
                queue->tail = prev;
            }
            
            free(current);
            queue->size--;
            return;
        }
        prev = current;
        current = current->next;
    }
}

PCB* queue_find_by_pid(Queue* queue, int pid) {
    if (!queue) return NULL;
    
    QueueNode* current = queue->head;
    while (current) {
        if (current->pcb && current->pcb->pid == pid) {
            return current->pcb;
        }
        current = current->next;
    }
    return NULL;
}

