#ifndef PCB_H
#define PCB_H

#include <stdbool.h>
#include <time.h>

typedef enum {
    STATE_NEW,
    STATE_READY,
    STATE_RUNNING,
    STATE_BLOCKED,
    STATE_TERMINATED
} ProcessState;

typedef struct PCB {
    int pid;
    ProcessState state;
    int priority;
    
    // Informations temporelles
    int arrival_time;
    int start_time;
    int finish_time;
    int remaining_time;
    int total_time;
    int wait_time;
    int blocked_time;
    
    // Contexte d'exécution (simulé)
    void* stack;
    int stack_size;
    
    // Ressources
    void* allocated_memory;
    int memory_size;
    int io_device;
    int io_end_time;
    
    // Synchronisation
    void* mutex_held;
    int semaphore_id;
    
    // Statistiques
    int context_switches;
    int last_run_time;
    
} PCB;

PCB* pcb_create(int pid, int priority, int total_time);
void pcb_destroy(PCB* pcb);
void pcb_set_state(PCB* pcb, ProcessState state);
const char* pcb_state_to_string(ProcessState state);
void pcb_print(PCB* pcb);

#endif // PCB_H

