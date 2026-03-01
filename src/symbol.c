#include "symbol.h"
#include <string.h>

void initSymbolTable(SymbolTable* table){
    table->count = 0;
    table->currentScopeDepth = 0;
    table->currentOffset = 0;
    //table->capacity = 1000;
}


void addSymbol(SymbolTable* table, const char* name, int length){
    table->currentOffset += 8;

    Symbol* sym = &table->symbols[table->count];
    sym->name = name;
    sym->length = length;
    sym->offset = table->currentOffset;
    sym->depth = table->currentScopeDepth;

    table->count++;
}

int getSymbolOffset(SymbolTable* table, const char* name, int length){
    for(int i = table->count - 1; i >= 0; i--){
        Symbol* sym = &table->symbols[i];
        if(sym->length == length && strncmp(sym->name, name, length) == 0){
            return sym->offset;
        }
    }
    return -1;
}

void beginScope(SymbolTable* table){
    table->currentScopeDepth++;
}

void endScope(SymbolTable* table) {
    while (table->count > 0 && table->symbols[table->count - 1].depth == table->currentScopeDepth) {
        table->count--;
    }
    table->currentScopeDepth--;
}