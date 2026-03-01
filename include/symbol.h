#ifndef SYMBOL_H
#define SYMBOL_H

typedef struct{
    const char* name;
    int length;
    int offset;
} Symbol;

typedef struct {
    Symbol symbols[256];
    int count;
    int currentOffset;
}SymbolTable;

void initSymbolTable(SymbolTable* table);
void addSymbol(SymbolTable* table, const char* name, int length);
int getSymbolOffset(SymbolTable* table, const char* name, int length);

#endif