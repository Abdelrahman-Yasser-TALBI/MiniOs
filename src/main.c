#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "pcb.h"
#include "scheduler.h"
#include "memory.h"
#include "io.h"
#include "sync.h"
#include "trace.h"
#include "queue.h"

#define MAX_PROCESSES 20
#define DEFAULT_QUANTUM 5
#define DEFAULT_TIME 100

// Variables globales du système
Scheduler* scheduler = NULL;
MemoryManager* memory_manager = NULL;
IOManager* io_manager = NULL;
TraceManager* trace_manager = NULL;
PCB* processes[MAX_PROCESSES];
int process_count = 0;
int current_time = 0;
int max_time = DEFAULT_TIME;

// Mutex et sémaphores globaux
Mutex* mutexes[5];
Semaphore* semaphores[5];
int mutex_count = 0;
int semaphore_count = 0;

void print_banner(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║          🖥️  MiniOS - Simulation d'OS                 ║\n");
    printf("║     Système d'exploitation en espace utilisateur      ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void print_help(const char* prog_name) {
    printf("Usage: %s [options]\n", prog_name);
    printf("\nOptions:\n");
    printf("  -a ALGO    Algorithme d'ordonnancement (fcfs, rr, priority)\n");
    printf("  -n NUM     Nombre de processus (défaut: 5)\n");
    printf("  -q QUANTUM Quantum pour Round Robin (défaut: 5)\n");
    printf("  -t TIME    Temps maximum de simulation (défaut: 100)\n");
    printf("  -h         Afficher cette aide\n");
    printf("\n");
}

SchedulerType parse_scheduler(const char* algo) {
    if (strcmp(algo, "fcfs") == 0) return SCHED_FCFS;
    if (strcmp(algo, "rr") == 0) return SCHED_RR;
    if (strcmp(algo, "priority") == 0) return SCHED_PRIORITY;
    return SCHED_FCFS; // Par défaut
}

void create_processes(int count) {
    srand(time(NULL));
    
    for (int i = 0; i < count && i < MAX_PROCESSES; i++) {
        int priority = rand() % 5 + 1; // Priorité entre 1 et 5
        int total_time = rand() % 30 + 10; // Temps d'exécution entre 10 et 40
        int arrival = rand() % 10; // Arrivée entre 0 et 9
        
        PCB* pcb = pcb_create(i + 1, priority, total_time);
        if (pcb) {
            pcb->arrival_time = arrival;
            processes[process_count++] = pcb;
        }
    }
}

void simulate_process_execution(PCB* pcb, int time_slice) {
    if (!pcb) return;
    
    int execution = (time_slice < pcb->remaining_time) ? time_slice : pcb->remaining_time;
    pcb->remaining_time -= execution;
    pcb->last_run_time += execution;
    
    // Simuler des opérations aléatoires
    int action = rand() % 100;
    
    if (action < 20 && pcb->remaining_time > 0) {
        // 20% de chance de demander une I/O
        int device = rand() % io_manager->device_count;
        int duration = rand() % 10 + 5;
        io_request(io_manager, pcb, device, duration, current_time);
        trace_event(trace_manager, current_time, pcb->pid, "IO_REQUEST", 
                   pcb_state_to_string(pcb->state), pcb->remaining_time, pcb->wait_time);
    } else if (action < 30 && mutex_count > 0 && pcb->remaining_time > 0) {
        // 10% de chance de demander un mutex
        int mutex_id = rand() % mutex_count;
        if (!mutex_is_locked(mutexes[mutex_id])) {
            mutex_lock(mutexes[mutex_id], pcb, current_time);
            trace_event(trace_manager, current_time, pcb->pid, "MUTEX_LOCK", 
                       pcb_state_to_string(pcb->state), pcb->remaining_time, pcb->wait_time);
        }
    } else if (action < 40 && semaphore_count > 0 && pcb->remaining_time > 0) {
        // 10% de chance d'utiliser un sémaphore
        int sem_id = rand() % semaphore_count;
        semaphore_wait(semaphores[sem_id], pcb, current_time);
        trace_event(trace_manager, current_time, pcb->pid, "SEM_WAIT", 
                   pcb_state_to_string(pcb->state), pcb->remaining_time, pcb->wait_time);
    }
}

void update_wait_times(void) {
    for (int i = 0; i < process_count; i++) {
        PCB* pcb = processes[i];
        if (pcb && pcb->state == STATE_READY) {
            pcb->wait_time++;
        } else if (pcb && pcb->state == STATE_BLOCKED) {
            pcb->blocked_time++;
        }
    }
}

void check_and_unblock_processes(void) {
    // Vérifier les I/O terminées
    io_update(io_manager, current_time);
    
    // Vérifier les processus bloqués sur I/O
    for (int i = 0; i < process_count; i++) {
        PCB* pcb = processes[i];
        if (pcb && pcb->state == STATE_BLOCKED && pcb->io_end_time > 0) {
            if (current_time >= pcb->io_end_time) {
                pcb->io_end_time = -1;
                pcb->io_device = -1;
                pcb_set_state(pcb, STATE_READY);
                scheduler_add_process(scheduler, pcb);
                trace_event(trace_manager, current_time, pcb->pid, "IO_COMPLETE", 
                           pcb_state_to_string(pcb->state), pcb->remaining_time, pcb->wait_time);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    print_banner();
    
    // Paramètres par défaut
    SchedulerType sched_type = SCHED_FCFS;
    int num_processes = 5;
    int quantum = DEFAULT_QUANTUM;
    max_time = DEFAULT_TIME;
    
    // Parsing des arguments
    int opt;
    while ((opt = getopt(argc, argv, "a:n:q:t:h")) != -1) {
        switch (opt) {
            case 'a':
                sched_type = parse_scheduler(optarg);
                break;
            case 'n':
                num_processes = atoi(optarg);
                if (num_processes < 1) num_processes = 5;
                if (num_processes > MAX_PROCESSES) num_processes = MAX_PROCESSES;
                break;
            case 'q':
                quantum = atoi(optarg);
                if (quantum < 1) quantum = DEFAULT_QUANTUM;
                break;
            case 't':
                max_time = atoi(optarg);
                if (max_time < 1) max_time = DEFAULT_TIME;
                break;
            case 'h':
                print_help(argv[0]);
                return 0;
            default:
                print_help(argv[0]);
                return 1;
        }
    }
    
    printf("⚙️  Configuration:\n");
    printf("   Algorithme: %s\n", scheduler_type_to_string(sched_type));
    printf("   Processus: %d\n", num_processes);
    printf("   Quantum: %d\n", quantum);
    printf("   Temps max: %d\n", max_time);
    printf("\n");
    
    // Initialisation du système
    scheduler = scheduler_create(sched_type, quantum);
    memory_manager = memory_init(MEMORY_SIZE);
    io_manager = io_init(4);
    trace_manager = trace_init("minios_trace.txt");
    
    if (!scheduler || !memory_manager || !io_manager || !trace_manager) {
        fprintf(stderr, "❌ Erreur lors de l'initialisation du système\n");
        return 1;
    }
    
    // Créer des mutex et sémaphores
    mutex_count = 3;
    for (int i = 0; i < mutex_count; i++) {
        mutexes[i] = mutex_create(i);
    }
    
    semaphore_count = 2;
    for (int i = 0; i < semaphore_count; i++) {
        semaphores[i] = semaphore_create(i, 2);
    }
    
    // Créer les processus
    create_processes(num_processes);
    printf("✅ %d processus créés\n\n", process_count);
    
    // Boucle principale de simulation
    printf("🚀 Démarrage de la simulation...\n\n");
    
    int active_processes = process_count;
    
    while (current_time < max_time && active_processes > 0) {
        // Ajouter les processus qui arrivent
        for (int i = 0; i < process_count; i++) {
            PCB* pcb = processes[i];
            if (pcb && pcb->state == STATE_NEW && pcb->arrival_time <= current_time) {
                pcb_set_state(pcb, STATE_READY);
                scheduler_add_process(scheduler, pcb);
                trace_event(trace_manager, current_time, pcb->pid, "ARRIVAL", 
                           pcb_state_to_string(pcb->state), pcb->remaining_time, pcb->wait_time);
            }
        }
        
        // Vérifier les processus débloqués
        check_and_unblock_processes();
        
        // Obtenir le prochain processus à exécuter
        PCB* current = scheduler_get_next(scheduler, current_time);
        
        if (current && current->state == STATE_RUNNING) {
            // Exécuter le processus
            int time_slice = (scheduler->type == SCHED_RR) ? 
                            scheduler->quantum_remaining : 
                            current->remaining_time;
            
            if (time_slice > current->remaining_time) {
                time_slice = current->remaining_time;
            }
            
            simulate_process_execution(current, time_slice);
            
            if (scheduler->type == SCHED_RR) {
                scheduler->quantum_remaining -= time_slice;
            }
            
            trace_event(trace_manager, current_time, current->pid, "EXECUTE", 
                       pcb_state_to_string(current->state), current->remaining_time, current->wait_time);
            
            // Vérifier si le processus est terminé
            if (current->remaining_time <= 0) {
                pcb_set_state(current, STATE_TERMINATED);
                current->finish_time = current_time + 1;
                active_processes--;
                trace_event(trace_manager, current_time + 1, current->pid, "TERMINATE", 
                           pcb_state_to_string(current->state), 0, current->wait_time);
                scheduler->current_process = NULL;
            } else if (current->state == STATE_BLOCKED) {
                // Le processus s'est bloqué (I/O, mutex, etc.)
                scheduler->current_process = NULL;
            } else if (scheduler->type == SCHED_RR && scheduler->quantum_remaining <= 0) {
                // Preemption pour Round Robin
                scheduler_preempt(scheduler);
            }
        }
        
        // Mettre à jour les temps d'attente
        update_wait_times();
        
        current_time++;
    }
    
    // Finalisation
    trace_finalize(trace_manager);
    
    printf("\n✅ Simulation terminée à t=%d\n", current_time);
    
    // Afficher les statistiques
    trace_print_summary(trace_manager, processes, process_count);
    
    memory_print_stats(memory_manager);
    io_print_status(io_manager);
    
    printf("\n📁 Trace sauvegardée dans traces/minios_trace.txt\n");
    printf("📊 Exécutez 'make visualize' pour générer les graphiques\n\n");
    
    // Nettoyage
    for (int i = 0; i < process_count; i++) {
        pcb_destroy(processes[i]);
    }
    
    for (int i = 0; i < mutex_count; i++) {
        mutex_destroy(mutexes[i]);
    }
    
    for (int i = 0; i < semaphore_count; i++) {
        semaphore_destroy(semaphores[i]);
    }
    
    scheduler_destroy(scheduler);
    memory_destroy(memory_manager);
    io_destroy(io_manager);
    trace_destroy(trace_manager);
    
    return 0;
}

