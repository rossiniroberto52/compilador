#ifndef SYMBOL_H
#define SYMBOL_H

typedef struct{
    const char* name;
    int length;
    int offset;
    int depth;
} Symbol;

typedef struct {
    Symbol symbols[512];
    //int capacity;
    int count;
    int currentScopeDepth;
    int currentOffset;
}SymbolTable;

void initSymbolTable(SymbolTable* table);
void addSymbol(SymbolTable* table, const char* name, int length);
int getSymbolOffset(SymbolTable* table, const char* name, int length);
void beginScope(SymbolTable* table);
void endScope(SymbolTable* table);

#endif