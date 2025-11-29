#include "pcb.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

PCB* pcb_create(int pid, int priority, int total_time) {
    PCB* pcb = (PCB*)malloc(sizeof(PCB));
    if (!pcb) {
        fprintf(stderr, "Erreur: Impossible d'allouer le PCB\n");
        return NULL;
    }
    
    pcb->pid = pid;
    pcb->state = STATE_NEW;
    pcb->priority = priority;
    pcb->arrival_time = 0;
    pcb->start_time = -1;
    pcb->finish_time = -1;
    pcb->remaining_time = total_time;
    pcb->total_time = total_time;
    pcb->wait_time = 0;
    pcb->blocked_time = 0;
    
    pcb->stack_size = 4096;
    pcb->stack = malloc(pcb->stack_size);
    
    pcb->allocated_memory = NULL;
    pcb->memory_size = 0;
    pcb->io_device = -1;
    pcb->io_end_time = -1;
    
    pcb->mutex_held = NULL;
    pcb->semaphore_id = -1;
    
    pcb->context_switches = 0;
    pcb->last_run_time = 0;
    
    return pcb;
}

void pcb_destroy(PCB* pcb) {
    if (!pcb) return;
    
    if (pcb->stack) {
        free(pcb->stack);
    }
    if (pcb->allocated_memory) {
        free(pcb->allocated_memory);
    }
    free(pcb);
}

void pcb_set_state(PCB* pcb, ProcessState state) {
    if (pcb) {
        pcb->state = state;
    }
}

const char* pcb_state_to_string(ProcessState state) {
    switch (state) {
        case STATE_NEW: return "NEW";
        case STATE_READY: return "READY";
        case STATE_RUNNING: return "RUNNING";
        case STATE_BLOCKED: return "BLOCKED";
        case STATE_TERMINATED: return "TERMINATED";
        default: return "UNKNOWN";
    }
}

void pcb_print(PCB* pcb) {
    if (!pcb) return;
    
    printf("PCB[PID=%d, State=%s, Priority=%d, Remaining=%d, Wait=%d]\n",
           pcb->pid,
           pcb_state_to_string(pcb->state),
           pcb->priority,
           pcb->remaining_time,
           pcb->wait_time);
}

