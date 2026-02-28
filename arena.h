#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>

typedef struct{
    char* memory;
    size_t capacity;
    size_t offset;
} Arena;

void initArena(Arena* arena, size_t capacity);
void* arenaAlloc(Arena* arena, size_t size);
void freeArena(Arena* arena);

#endif