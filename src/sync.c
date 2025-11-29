#include "sync.h"
#include <stdlib.h>
#include <stdio.h>

Mutex* mutex_create(int id) {
    Mutex* mutex = (Mutex*)malloc(sizeof(Mutex));
    if (!mutex) {
        fprintf(stderr, "Erreur: Impossible d'allouer le mutex\n");
        return NULL;
    }
    
    mutex->id = id;
    mutex->locked = false;
    mutex->owner = NULL;
    mutex->wait_queue = queue_create();
    
    return mutex;
}

void mutex_destroy(Mutex* mutex) {
    if (!mutex) return;
    
    if (mutex->wait_queue) {
        queue_destroy(mutex->wait_queue);
    }
    free(mutex);
}

bool mutex_lock(Mutex* mutex, PCB* pcb, int current_time) {
    (void)current_time; // Paramètre réservé pour usage futur
    if (!mutex || !pcb) return false;
    
    if (!mutex->locked) {
        mutex->locked = true;
        mutex->owner = pcb;
        pcb->mutex_held = mutex;
        return true;
    } else {
        // Le mutex est déjà verrouillé, bloquer le processus
        queue_enqueue(mutex->wait_queue, pcb);
        pcb_set_state(pcb, STATE_BLOCKED);
        return false;
    }
}

bool mutex_unlock(Mutex* mutex, int current_time) {
    (void)current_time; // Paramètre réservé pour usage futur
    if (!mutex || !mutex->locked) return false;
    
    mutex->locked = false;
    if (mutex->owner) {
        mutex->owner->mutex_held = NULL;
    }
    mutex->owner = NULL;
    
    // Débloquer le prochain processus en attente
    PCB* next = queue_dequeue(mutex->wait_queue);
    if (next) {
        mutex->locked = true;
        mutex->owner = next;
        next->mutex_held = mutex;
        pcb_set_state(next, STATE_READY);
        return true;
    }
    
    return true;
}

bool mutex_is_locked(Mutex* mutex) {
    return mutex && mutex->locked;
}

Semaphore* semaphore_create(int id, int initial_count) {
    Semaphore* sem = (Semaphore*)malloc(sizeof(Semaphore));
    if (!sem) {
        fprintf(stderr, "Erreur: Impossible d'allouer le sémaphore\n");
        return NULL;
    }
    
    sem->id = id;
    sem->count = initial_count;
    sem->max_count = initial_count;
    sem->wait_queue = queue_create();
    
    return sem;
}

void semaphore_destroy(Semaphore* sem) {
    if (!sem) return;
    
    if (sem->wait_queue) {
        queue_destroy(sem->wait_queue);
    }
    free(sem);
}

bool semaphore_wait(Semaphore* sem, PCB* pcb, int current_time) {
    (void)current_time; // Paramètre réservé pour usage futur
    if (!sem || !pcb) return false;
    
    if (sem->count > 0) {
        sem->count--;
        pcb->semaphore_id = sem->id;
        return true;
    } else {
        // Pas de jetons disponibles, bloquer le processus
        queue_enqueue(sem->wait_queue, pcb);
        pcb_set_state(pcb, STATE_BLOCKED);
        pcb->semaphore_id = sem->id;
        return false;
    }
}

bool semaphore_signal(Semaphore* sem, int current_time) {
    (void)current_time; // Paramètre réservé pour usage futur
    if (!sem) return false;
    
    if (!queue_is_empty(sem->wait_queue)) {
        // Débloquer un processus en attente
        PCB* pcb = queue_dequeue(sem->wait_queue);
        if (pcb) {
            pcb_set_state(pcb, STATE_READY);
            pcb->semaphore_id = -1;
            return true;
        }
    } else {
        // Aucun processus en attente, incrémenter le compteur
        if (sem->count < sem->max_count) {
            sem->count++;
        }
    }
    
    return true;
}

int semaphore_get_count(Semaphore* sem) {
    return sem ? sem->count : -1;
}

