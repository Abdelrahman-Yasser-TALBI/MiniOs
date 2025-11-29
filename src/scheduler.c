#include "scheduler.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Scheduler* scheduler_create(SchedulerType type, int quantum) {
    Scheduler* sched = (Scheduler*)malloc(sizeof(Scheduler));
    if (!sched) {
        fprintf(stderr, "Erreur: Impossible d'allouer l'ordonnanceur\n");
        return NULL;
    }
    
    sched->type = type;
    sched->ready_queue = queue_create();
    sched->quantum = quantum;
    sched->quantum_remaining = quantum;
    sched->current_process = NULL;
    sched->total_context_switches = 0;
    
    return sched;
}

void scheduler_destroy(Scheduler* sched) {
    if (!sched) return;
    
    if (sched->ready_queue) {
        queue_destroy(sched->ready_queue);
    }
    free(sched);
}

void scheduler_add_process(Scheduler* sched, PCB* pcb) {
    if (!sched || !pcb) return;
    
    // Pour Priority, insérer selon la priorité (plus haute d'abord)
    if (sched->type == SCHED_PRIORITY) {
        QueueNode* current = sched->ready_queue->head;
        QueueNode* prev = NULL;
        QueueNode* new_node = (QueueNode*)malloc(sizeof(QueueNode));
        new_node->pcb = pcb;
        new_node->next = NULL;
        
        // Trouver la position d'insertion
        while (current && current->pcb->priority >= pcb->priority) {
            prev = current;
            current = current->next;
        }
        
        if (prev) {
            new_node->next = prev->next;
            prev->next = new_node;
        } else {
            new_node->next = sched->ready_queue->head;
            sched->ready_queue->head = new_node;
        }
        
        if (!current) {
            sched->ready_queue->tail = new_node;
        }
        sched->ready_queue->size++;
    } else {
        // FCFS et RR: ajout en fin de file
        queue_enqueue(sched->ready_queue, pcb);
    }
}

PCB* scheduler_get_next(Scheduler* sched, int current_time) {
    if (!sched) return NULL;
    
    // Si un processus est en cours et qu'on utilise Round Robin
    if (sched->current_process && sched->type == SCHED_RR) {
        if (sched->quantum_remaining > 0 && 
            sched->current_process->remaining_time > 0 &&
            sched->current_process->state == STATE_RUNNING) {
            // Continuer avec le processus actuel
            return sched->current_process;
        }
    }
    
    // Prendre le prochain processus de la file
    PCB* next = queue_dequeue(sched->ready_queue);
    if (next) {
        // Sauvegarder l'ancien processus s'il existe
        if (sched->current_process && 
            sched->current_process != next &&
            sched->current_process->state == STATE_RUNNING) {
            // Remettre l'ancien processus dans la file (pour RR)
            if (sched->type == SCHED_RR && sched->current_process->remaining_time > 0) {
                scheduler_add_process(sched, sched->current_process);
            }
            sched->current_process->context_switches++;
        }
        
        sched->current_process = next;
        sched->quantum_remaining = sched->quantum;
        sched->total_context_switches++;
        
        if (next->start_time == -1) {
            next->start_time = current_time;
        }
        
        pcb_set_state(next, STATE_RUNNING);
    }
    
    return next;
}

void scheduler_preempt(Scheduler* sched) {
    if (!sched || !sched->current_process) return;
    
    if (sched->type == SCHED_RR) {
        if (sched->quantum_remaining <= 0 && 
            sched->current_process->remaining_time > 0) {
            // Remettre le processus dans la file
            PCB* pcb = sched->current_process;
            pcb_set_state(pcb, STATE_READY);
            scheduler_add_process(sched, pcb);
            sched->current_process = NULL;
        }
    }
}

bool scheduler_has_ready_processes(Scheduler* sched) {
    return sched && !queue_is_empty(sched->ready_queue);
}

int scheduler_get_ready_count(Scheduler* sched) {
    return sched ? queue_size(sched->ready_queue) : 0;
}

const char* scheduler_type_to_string(SchedulerType type) {
    switch (type) {
        case SCHED_FCFS: return "FCFS (First Come First Served)";
        case SCHED_RR: return "Round Robin";
        case SCHED_PRIORITY: return "Priority Scheduling";
        default: return "Unknown";
    }
}

