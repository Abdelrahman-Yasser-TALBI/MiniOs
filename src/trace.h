#ifndef TRACE_H
#define TRACE_H

#include "pcb.h"
#include <stdio.h>

typedef struct TraceEvent {
    int time;
    int pid;
    const char* event_type;
    const char* state;
    int remaining_time;
    int wait_time;
} TraceEvent;

typedef struct TraceManager {
    FILE* trace_file;
    TraceEvent* events;
    int event_count;
    int event_capacity;
    int total_processes;
    int total_time;
} TraceManager;

TraceManager* trace_init(const char* filename);
void trace_destroy(TraceManager* tm);
void trace_event(TraceManager* tm, int time, int pid, const char* event_type, 
                 const char* state, int remaining_time, int wait_time);
void trace_finalize(TraceManager* tm);
void trace_print_summary(TraceManager* tm, PCB** processes, int process_count);

#endif // TRACE_H

