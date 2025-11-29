#include "memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

MemoryManager* memory_init(size_t size) {
    MemoryManager* mm = (MemoryManager*)malloc(sizeof(MemoryManager));
    if (!mm) {
        fprintf(stderr, "Erreur: Impossible d'allouer le gestionnaire mémoire\n");
        return NULL;
    }
    
    mm->heap = (char*)malloc(size);
    if (!mm->heap) {
        free(mm);
        fprintf(stderr, "Erreur: Impossible d'allouer le heap\n");
        return NULL;
    }
    
    mm->total_size = size;
    mm->allocations = 0;
    mm->deallocations = 0;
    
    // Initialiser la free list avec tout le heap
    mm->free_list = (MemoryBlock*)malloc(sizeof(MemoryBlock));
    if (!mm->free_list) {
        free(mm->heap);
        free(mm);
        return NULL;
    }
    
    mm->free_list->start = mm->heap;
    mm->free_list->size = size;
    mm->free_list->is_free = true;
    mm->free_list->next = NULL;
    
    return mm;
}

void memory_destroy(MemoryManager* mm) {
    if (!mm) return;
    
    MemoryBlock* current = mm->free_list;
    while (current) {
        MemoryBlock* next = current->next;
        free(current);
        current = next;
    }
    
    if (mm->heap) {
        free(mm->heap);
    }
    free(mm);
}

static MemoryBlock* find_block(MemoryManager* mm, void* ptr) {
    MemoryBlock* current = mm->free_list;
    while (current) {
        if (current->start == ptr) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void* memory_allocate(MemoryManager* mm, size_t size) {
    if (!mm || size == 0) return NULL;
    
    // Alignement sur 8 octets
    size = (size + 7) & ~7;
    
    MemoryBlock* best = NULL;
    MemoryBlock* current = mm->free_list;
    
    // First-fit algorithm
    while (current) {
        if (current->is_free && current->size >= size) {
            if (!best || current->size < best->size) {
                best = current;
            }
        }
        current = current->next;
    }
    
    if (!best) {
        return NULL; // Pas assez de mémoire
    }
    
    // Si le bloc est plus grand, le diviser
    if (best->size > size + sizeof(MemoryBlock)) {
        MemoryBlock* new_block = (MemoryBlock*)malloc(sizeof(MemoryBlock));
        if (!new_block) {
            return NULL;
        }
        
        new_block->start = (char*)best->start + size;
        new_block->size = best->size - size;
        new_block->is_free = true;
        new_block->next = best->next;
        best->next = new_block;
        best->size = size;
    }
    
    best->is_free = false;
    mm->allocations++;
    
    return best->start;
}

bool memory_free(MemoryManager* mm, void* ptr) {
    if (!mm || !ptr) return false;
    
    MemoryBlock* block = find_block(mm, ptr);
    if (!block || block->is_free) {
        return false; // Double libération ou pointeur invalide
    }
    
    block->is_free = true;
    mm->deallocations++;
    
    // Fusionner avec les blocs adjacents libres
    MemoryBlock* current = mm->free_list;
    while (current) {
        if (current->is_free && current != block) {
            char* current_end = (char*)current->start + current->size;
            char* block_start = (char*)block->start;
            char* block_end = (char*)block->start + block->size;
            
            // Fusionner avec le bloc suivant
            if (current_end == block_start) {
                current->size += block->size;
                current->next = block->next;
                free(block);
                return true;
            }
            // Fusionner avec le bloc précédent
            else if (block_end == current->start) {
                block->size += current->size;
                block->next = current->next;
                if (mm->free_list == current) {
                    mm->free_list = block;
                }
                free(current);
                return true;
            }
        }
        current = current->next;
    }
    
    return true;
}

void memory_print_stats(MemoryManager* mm) {
    if (!mm) return;
    
    size_t free = memory_get_free_space(mm);
    size_t used = memory_get_used_space(mm);
    
    printf("📊 Statistiques mémoire:\n");
    printf("   Total: %zu bytes\n", mm->total_size);
    printf("   Utilisé: %zu bytes (%.1f%%)\n", used, (used * 100.0 / mm->total_size));
    printf("   Libre: %zu bytes (%.1f%%)\n", free, (free * 100.0 / mm->total_size));
    printf("   Allocations: %d\n", mm->allocations);
    printf("   Désallocations: %d\n", mm->deallocations);
}

size_t memory_get_free_space(MemoryManager* mm) {
    if (!mm) return 0;
    
    size_t free = 0;
    MemoryBlock* current = mm->free_list;
    while (current) {
        if (current->is_free) {
            free += current->size;
        }
        current = current->next;
    }
    return free;
}

size_t memory_get_used_space(MemoryManager* mm) {
    if (!mm) return 0;
    return mm->total_size - memory_get_free_space(mm);
}

