#ifndef IO_H
#define IO_H

#include "pcb.h"
#include "queue.h"

#define MAX_IO_DEVICES 4

typedef struct IODevice {
    int device_id;
    bool is_busy;
    PCB* current_process;
    int end_time;
} IODevice;

typedef struct IOManager {
    IODevice devices[MAX_IO_DEVICES];
    Queue* io_queue;
    int device_count;
} IOManager;

IOManager* io_init(int device_count);
void io_destroy(IOManager* io);
bool io_request(IOManager* io, PCB* pcb, int device_id, int duration, int current_time);
void io_update(IOManager* io, int current_time);
bool io_is_device_busy(IOManager* io, int device_id);
void io_print_status(IOManager* io);

#endif // IO_H

