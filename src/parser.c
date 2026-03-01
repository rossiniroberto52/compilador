#include "parser.h"
#include "symbol.h"
#include <stdio.h>
#include <stdlib.h>

extern Token currentToken;
extern void advanceToken();
extern void consume(TokenType type, const char* message);

static ASTNode* parseTerm(Arena* arena);
static ASTNode* parseFactor(Arena* arena);

ASTNode* parseExpression(Arena* arena){
    ASTNode* node = parseTerm(arena);
    while(currentToken.type == TOKEN_PLUS || currentToken.type == TOKEN_MINUS){
        TokenType  operator =  currentToken.type;
        advanceToken();

        ASTNode* rightnode = parseTerm(arena);

        ASTNode* binaryNode = (ASTNode*)arenaAlloc(arena, sizeof(ASTNode));

        binaryNode->type = NODE_BINARY_OP;
        binaryNode->as.binaryOp.left = node;
        binaryNode->as.binaryOp.right = rightnode;
        binaryNode->as.binaryOp.operator = operator;
        node = binaryNode;
    }
    return node;
}

static ASTNode* parseTerm(Arena* arena){
    ASTNode* node = parseFactor(arena);

    while(currentToken.type == TOKEN_STAR || currentToken.type == TOKEN_SLASH){
        TokenType operator = currentToken.type;
        advanceToken();

        ASTNode* rigthtNode = parseFactor(arena);

        ASTNode* binaryNode = (ASTNode*)arenaAlloc(arena, sizeof(ASTNode));
        
        binaryNode->type = NODE_BINARY_OP;
        binaryNode->as.binaryOp.left = node;
        binaryNode-> as.binaryOp.right = rigthtNode;
        binaryNode->as.binaryOp.operator = operator;

        node = binaryNode;
    }

    return node;
}

static ASTNode* parseFactor(Arena* arena){
    if(currentToken.type == TOKEN_NUMBER){
        ASTNode* node = (ASTNode*)arenaAlloc(arena, sizeof(ASTNode));
        node->type = NODE_NUMBER;

        char buffer[64];
        snprintf(buffer, sizeof(buffer), "%.*s", currentToken.length, currentToken.start);
        node->as.numberValue = atoi(buffer);
        advanceToken();
        return node;
    }
    if(currentToken.type == TOKEN_IDENTIFIER){
        ASTNode* node = (ASTNode*)arenaAlloc(arena, sizeof(ASTNode));
        node->type = NODE_IDENTIFIER;

        node->as.identifier.name = currentToken.start;
        node->as.identifier.length = currentToken.length;

        advanceToken();
        return node;
    }

    if(currentToken.type == TOKEN_LPAREN){
        advanceToken();
        ASTNode* node = parseExpression(arena);
        consume(TOKEN_RPAREN, "Expected ')' after expression");
        return node;
    }

    fprintf(stderr, "Error: Expected expression at line %d.\n", currentToken.line);
    exit(65);
}

ASTNode* parseStatement(Arena* arena, SymbolTable* table){
    if(currentToken.type == TOKEN_IDENTIFIER){
        const char* varName = currentToken.start;
        int varLength = currentToken.length;

        advanceToken();

        if(currentToken.type == TOKEN_ASSIGN){
            advanceToken();
            if(getSymbolOffset(table, varName, varLength) == -1){
                addSymbol(table, varName, varLength);
            }

            ASTNode* exprNode = parseExpression(arena);

            consume(TOKEN_SEMICOLON, "Expected ';' after expression");

            ASTNode* assignNode = (ASTNode*)arenaAlloc(arena, sizeof(ASTNode));
            assignNode->type = NODE_ASSIGN;
            assignNode->as.assign.name = varName;
            assignNode->as.assign.length = varLength;
            assignNode->as.assign.expr = exprNode;

            return assignNode;
        }
    }

    return parseExpression(arena);
}

void printAST(ASTNode* node, int depth){
    if(node == NULL) return;
    for(int i = 0; i < depth; i++){
        fprintf(stderr, "  ");
    }

    switch(node->type){
        case NODE_NUMBER:
            fprintf(stderr, "Number: %d\n", node->as.numberValue);
            break;

        case NODE_IDENTIFIER:
            fprintf(stderr, "Variable: %.*s\n", node->as.identifier.length, node->as.identifier.name);
            break;
        case NODE_BINARY_OP:{
            char op = '?';
            if(node->as.binaryOp.operator == TOKEN_PLUS) op = '+';
            else if(node->as.binaryOp.operator == TOKEN_MINUS) op = '-';
            else if(node->as.binaryOp.operator == TOKEN_STAR) op = '*';
            else if(node->as.binaryOp.operator == TOKEN_SLASH) op = '/';
            fprintf(stderr, "BinaryOp: [%c]\n", op);
            
            printAST(node->as.binaryOp.left, depth + 1);
            printAST(node->as.binaryOp.right, depth + 1);
            break;
        }
        case NODE_ASSIGN:
            fprintf(stderr, "Assign: %.*s\n", node->as.assign.length, node->as.assign.name);
            printAST(node->as.assign.expr, depth + 1);
            break;
        default:
            fprintf(stderr, "Unknown node type\n");
    }
}