#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "arena.h" 

typedef enum{
    NODE_NUMBER,
    NODE_IDENTIFIER,
    NODE_BINARY_OP,
    NODE_ASSIGN,
    NODE_IF,
    NODE_WHILE,
    NODE_PRINT
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;

    union{
        int numberValue;

        struct
        {
            const char* name;
            int length;
        } identifier;

        struct {
            struct ASTNode* left;
            struct ASTNode* right;
            TokenType operator;
        } binaryOp;

        struct {
            const char* name;
            int length;
            struct ASTNode* value;
        } assign;

        struct{
            struct ASTNode* condition;
            struct ASTNode* body;
        } controlFlow;

        struct{
            struct ASTNode* expression;
        } print;
        
    } as;
} ASTNode;


ASTNode* parseExpression(Arena* arena);
void printAST(ASTNode* node, int indent);

#endif