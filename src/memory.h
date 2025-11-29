#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>
#include <stddef.h>

#define MEMORY_SIZE (1024 * 1024) // 1 MB de mémoire simulée

typedef struct MemoryBlock {
    void* start;
    size_t size;
    bool is_free;
    struct MemoryBlock* next;
} MemoryBlock;

typedef struct MemoryManager {
    char* heap;
    size_t total_size;
    MemoryBlock* free_list;
    int allocations;
    int deallocations;
} MemoryManager;

MemoryManager* memory_init(size_t size);
void memory_destroy(MemoryManager* mm);
void* memory_allocate(MemoryManager* mm, size_t size);
bool memory_free(MemoryManager* mm, void* ptr);
void memory_print_stats(MemoryManager* mm);
size_t memory_get_free_space(MemoryManager* mm);
size_t memory_get_used_space(MemoryManager* mm);

#endif // MEMORY_H

