#include "symbol.h"
#include <string.h>

void initSymbolTable(SymbolTable* table){
    table->count = 0;
    table->currentOffset = 0;
}

void addSymbol(SymbolTable* table, const char* name, int length){
    table->currentOffset += 8;

    Symbol* sym = &table->symbols[table->count];
    sym->name = name;
    sym->length = length;
    sym->offset = table->currentOffset;

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