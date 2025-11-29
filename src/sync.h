#ifndef SYNC_H
#define SYNC_H

#include <stdbool.h>
#include "pcb.h"
#include "queue.h"

typedef struct Mutex {
    int id;
    bool locked;
    PCB* owner;
    Queue* wait_queue;
} Mutex;

typedef struct Semaphore {
    int id;
    int count;
    int max_count;
    Queue* wait_queue;
} Semaphore;

Mutex* mutex_create(int id);
void mutex_destroy(Mutex* mutex);
bool mutex_lock(Mutex* mutex, PCB* pcb, int current_time);
bool mutex_unlock(Mutex* mutex, int current_time);
bool mutex_is_locked(Mutex* mutex);

Semaphore* semaphore_create(int id, int initial_count);
void semaphore_destroy(Semaphore* sem);
bool semaphore_wait(Semaphore* sem, PCB* pcb, int current_time);
bool semaphore_signal(Semaphore* sem, int current_time);
int semaphore_get_count(Semaphore* sem);

#endif // SYNC_H

