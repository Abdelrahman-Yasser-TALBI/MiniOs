#include "trace.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INITIAL_CAPACITY 1000

TraceManager* trace_init(const char* filename) {
    TraceManager* tm = (TraceManager*)malloc(sizeof(TraceManager));
    if (!tm) {
        fprintf(stderr, "Erreur: Impossible d'allouer le gestionnaire de traces\n");
        return NULL;
    }
    
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "traces/%s", filename);
    
    tm->trace_file = fopen(filepath, "w");
    if (!tm->trace_file) {
        free(tm);
        fprintf(stderr, "Erreur: Impossible de créer le fichier de trace\n");
        return NULL;
    }
    
    tm->events = (TraceEvent*)malloc(sizeof(TraceEvent) * INITIAL_CAPACITY);
    if (!tm->events) {
        fclose(tm->trace_file);
        free(tm);
        return NULL;
    }
    
    tm->event_count = 0;
    tm->event_capacity = INITIAL_CAPACITY;
    tm->total_processes = 0;
    tm->total_time = 0;
    
    // En-tête du fichier de trace
    fprintf(tm->trace_file, "=== MiniOS Trace File ===\n");
    fprintf(tm->trace_file, "Format: Time | PID | Event | State | Remaining | Wait\n");
    fprintf(tm->trace_file, "==========================================\n");
    
    return tm;
}

void trace_destroy(TraceManager* tm) {
    if (!tm) return;
    
    if (tm->trace_file) {
        fclose(tm->trace_file);
    }
    
    if (tm->events) {
        free(tm->events);
    }
    
    free(tm);
}

void trace_event(TraceManager* tm, int time, int pid, const char* event_type, 
                 const char* state, int remaining_time, int wait_time) {
    if (!tm) return;
    
    // Agrandir le tableau si nécessaire
    if (tm->event_count >= tm->event_capacity) {
        tm->event_capacity *= 2;
        tm->events = (TraceEvent*)realloc(tm->events, 
                                          sizeof(TraceEvent) * tm->event_capacity);
        if (!tm->events) {
            fprintf(stderr, "Erreur: Impossible d'agrandir le tableau de traces\n");
            return;
        }
    }
    
    TraceEvent* event = &tm->events[tm->event_count++];
    event->time = time;
    event->pid = pid;
    event->event_type = event_type;
    event->state = state;
    event->remaining_time = remaining_time;
    event->wait_time = wait_time;
    
    // Écrire dans le fichier
    fprintf(tm->trace_file, "%d | %d | %s | %s | %d | %d\n",
            time, pid, event_type, state, remaining_time, wait_time);
    fflush(tm->trace_file);
}

void trace_finalize(TraceManager* tm) {
    if (!tm || !tm->trace_file) return;
    
    fprintf(tm->trace_file, "==========================================\n");
    fprintf(tm->trace_file, "Total events: %d\n", tm->event_count);
    fflush(tm->trace_file);
}

void trace_print_summary(TraceManager* tm, PCB** processes, int process_count) {
    if (!tm || !processes) return;
    
    printf("\n📊 RÉSUMÉ DE L'EXÉCUTION\n");
    printf("========================\n");
    
    double total_turnaround = 0;
    double total_response = 0;
    double total_wait = 0;
    int completed = 0;
    
    for (int i = 0; i < process_count; i++) {
        PCB* pcb = processes[i];
        if (pcb && pcb->finish_time > 0) {
            int turnaround = pcb->finish_time - pcb->arrival_time;
            int response = pcb->start_time - pcb->arrival_time;
            
            total_turnaround += turnaround;
            total_response += response;
            total_wait += pcb->wait_time;
            completed++;
            
            printf("PID %d: Arrival=%d, Start=%d, Finish=%d, "
                   "Turnaround=%d, Response=%d, Wait=%d\n",
                   pcb->pid, pcb->arrival_time, pcb->start_time,
                   pcb->finish_time, turnaround, response, pcb->wait_time);
        }
    }
    
    if (completed > 0) {
        printf("\n📈 STATISTIQUES GLOBALES:\n");
        printf("   Temps moyen de retour: %.2f\n", total_turnaround / completed);
        printf("   Temps moyen de réponse: %.2f\n", total_response / completed);
        printf("   Temps moyen d'attente: %.2f\n", total_wait / completed);
        printf("   Changements de contexte: %d\n", tm->event_count);
    }
}

