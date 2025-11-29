#include "io.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

IOManager* io_init(int device_count) {
    if (device_count > MAX_IO_DEVICES) {
        device_count = MAX_IO_DEVICES;
    }
    
    IOManager* io = (IOManager*)malloc(sizeof(IOManager));
    if (!io) {
        fprintf(stderr, "Erreur: Impossible d'allouer le gestionnaire I/O\n");
        return NULL;
    }
    
    io->device_count = device_count;
    io->io_queue = queue_create();
    
    for (int i = 0; i < device_count; i++) {
        io->devices[i].device_id = i;
        io->devices[i].is_busy = false;
        io->devices[i].current_process = NULL;
        io->devices[i].end_time = -1;
    }
    
    return io;
}

void io_destroy(IOManager* io) {
    if (!io) return;
    
    if (io->io_queue) {
        queue_destroy(io->io_queue);
    }
    free(io);
}

bool io_request(IOManager* io, PCB* pcb, int device_id, int duration, int current_time) {
    if (!io || !pcb || device_id < 0 || device_id >= io->device_count) {
        return false;
    }
    
    pcb->io_device = device_id;
    pcb->io_end_time = current_time + duration;
    
    if (!io->devices[device_id].is_busy) {
        // Le périphérique est libre, commencer immédiatement
        io->devices[device_id].is_busy = true;
        io->devices[device_id].current_process = pcb;
        io->devices[device_id].end_time = current_time + duration;
        pcb_set_state(pcb, STATE_BLOCKED);
        return true;
    } else {
        // Le périphérique est occupé, mettre en file d'attente
        queue_enqueue(io->io_queue, pcb);
        pcb_set_state(pcb, STATE_BLOCKED);
        return false;
    }
}

void io_update(IOManager* io, int current_time) {
    if (!io) return;
    
    // Vérifier chaque périphérique
    for (int i = 0; i < io->device_count; i++) {
        if (io->devices[i].is_busy && 
            io->devices[i].end_time <= current_time) {
            
            // L'opération I/O est terminée
            PCB* pcb = io->devices[i].current_process;
            if (pcb) {
                pcb->io_device = -1;
                pcb->io_end_time = -1;
                pcb_set_state(pcb, STATE_READY);
            }
            
            io->devices[i].is_busy = false;
            io->devices[i].current_process = NULL;
            io->devices[i].end_time = -1;
            
            // Prendre le prochain processus de la file
            PCB* next = queue_dequeue(io->io_queue);
            if (next && next->io_device == i) {
                io->devices[i].is_busy = true;
                io->devices[i].current_process = next;
                io->devices[i].end_time = next->io_end_time;
            }
        }
    }
}

bool io_is_device_busy(IOManager* io, int device_id) {
    if (!io || device_id < 0 || device_id >= io->device_count) {
        return false;
    }
    return io->devices[device_id].is_busy;
}

void io_print_status(IOManager* io) {
    if (!io) return;
    
    printf("🔌 État des périphériques I/O:\n");
    for (int i = 0; i < io->device_count; i++) {
        if (io->devices[i].is_busy) {
            printf("   Device %d: Occupé par PID %d (fin à t=%d)\n",
                   i, io->devices[i].current_process->pid, io->devices[i].end_time);
        } else {
            printf("   Device %d: Libre\n", i);
        }
    }
    printf("   Processus en attente I/O: %d\n", queue_size(io->io_queue));
}

