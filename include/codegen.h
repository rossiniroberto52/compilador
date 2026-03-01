#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"
#include "symbol.h"

void generateAssembly(ASTNode* node, SymbolTable* table);

#endif