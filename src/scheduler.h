#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "pcb.h"
#include "queue.h"

typedef enum {
    SCHED_FCFS,      // First Come First Served
    SCHED_RR,        // Round Robin
    SCHED_PRIORITY    // Priority Scheduling
} SchedulerType;

typedef struct Scheduler {
    SchedulerType type;
    Queue* ready_queue;
    int quantum;
    int quantum_remaining;
    PCB* current_process;
    int total_context_switches;
} Scheduler;

Scheduler* scheduler_create(SchedulerType type, int quantum);
void scheduler_destroy(Scheduler* sched);
void scheduler_add_process(Scheduler* sched, PCB* pcb);
PCB* scheduler_get_next(Scheduler* sched, int current_time);
void scheduler_preempt(Scheduler* sched);
bool scheduler_has_ready_processes(Scheduler* sched);
int scheduler_get_ready_count(Scheduler* sched);
const char* scheduler_type_to_string(SchedulerType type);

#endif // SCHEDULER_H

