#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "symbol.h"
#include "arena.h" 

typedef enum{
    NODE_NUMBER,
    NODE_IDENTIFIER,
    NODE_BINARY_OP,
    NODE_ASSIGN,
    NODE_IF,
    NODE_WHILE,
    NODE_BLOCK,
    NODE_LOGICAL_AND,
    NODE_LOGICAL_OR,
    NODE_PRINT
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    struct ASTNode* next;

    union{
        int numberValue;

        struct
        {
            const char* name;
            int length;
            int offset;
        } identifier;

        struct {
            struct ASTNode* left;
            struct ASTNode* right;
            TokenType operator;
        } binaryOp;

        struct {
            const char* name;
            int length;
            struct ASTNode* expr;
            int offset;
        } assign;

        struct{
            struct ASTNode* condition;
            struct ASTNode* body;
        } controlFlow;

        struct {
            struct ASTNode* head;
        } block;
        
        struct{
            struct ASTNode* expression;
        } print; 
        
    } as;
} ASTNode;


ASTNode* parseExpression(Arena* arena, SymbolTable* table);
ASTNode* parseStatement(Arena* arena, SymbolTable* table);
void printAST(ASTNode* node, int indent);

#endif