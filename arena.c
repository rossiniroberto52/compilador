#include "arena.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

void initArena(Arena* arena, size_t capacity){
    arena->memory = (char*)malloc(capacity);
    if(arena->memory == NULL){
        fprintf(stderr, "Error: Failed to allocate mem arena.\n");
        exit(74);
    }
    arena->capacity = capacity;
    arena->offset = 0;
}

void* arenaAlloc(Arena* arena, size_t size){
    if(arena->offset+size > arena->capacity){
        fprintf(stderr, "Error: Memory overflow");
        exit(74);
    }

    size = (size+7) & ~7;

    void* mem = arena->memory + arena->offset;
    arena->offset += size;

    return mem;
}

void freeArena(Arena* arena){
    free(arena->memory);
    arena->memory = NULL;
    arena->offset = 0;
    arena->capacity = 0;
}