#include "parser.h"
#include "symbol.h"
#include <stdio.h>
#include <stdlib.h>

extern Token currentToken;
extern void advanceToken();
extern void consume(TokenType type, const char* message);

ASTNode* parseBlock(Arena* arena, SymbolTable* table);

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


ASTNode* parseEquality(Arena* arena){
    ASTNode* left = parseExpression(arena);

    while(currentToken.type == TOKEN_EQUAL_EQUAL){
        TokenType operatorType = currentToken.type;
        advanceToken();

        ASTNode* right = parseExpression(arena);

        ASTNode* node = (ASTNode*)arenaAlloc(arena, sizeof(ASTNode));
        node->type = NODE_BINARY_OP;
        node->as.binaryOp.operator = operatorType;
        node->as.binaryOp.left = left;
        node->as.binaryOp.right = right;

        left = node;
    }

    return left;
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

ASTNode* parseBlock(Arena* arena, SymbolTable* table){
    consume(TOKEN_LBRACE, "Expected '{' at the beginning of block");
    ASTNode* blockNode = (ASTNode*)arenaAlloc(arena, sizeof(ASTNode));
    blockNode->type = NODE_BLOCK;
    blockNode->as.block.head = NULL;

    ASTNode** current = &blockNode->as.block.head;

    while(currentToken.type != TOKEN_RBRACE && currentToken.type != TOKEN_EOF){
        ASTNode* statement = parseStatement(arena, table);
        *current = statement;
        current = &statement->next; 
    }

    consume(TOKEN_RBRACE, "Expected '}' at the end of block");
    return blockNode;
}


ASTNode* parseStatement(Arena* arena, SymbolTable* table){
    if(currentToken.type == TOKEN_PRINT){
        advanceToken();
        consume(TOKEN_LPAREN, "Expected '(' after 'print'");
        ASTNode* exprNode = parseEquality(arena);
        consume(TOKEN_RPAREN, "Expected ')' after expression");
        consume(TOKEN_SEMICOLON, "Expected ';' after print statement");
        ASTNode* printNode = (ASTNode*)arenaAlloc(arena, sizeof(ASTNode));
        printNode->type = NODE_PRINT;
        printNode->as.print.expression = exprNode;

        return printNode;
    }

    if (currentToken.type == TOKEN_IF){
        advanceToken();

        consume(TOKEN_LPAREN, "Expected '(' after 'if'");
        ASTNode* conditionNode = parseEquality(arena);
        consume(TOKEN_RPAREN, "Expected ')' after condition");

        consume(TOKEN_LBRACE, "Expected '{' before if body");
        ASTNode* bodyNode = parseStatement(arena, table);
        consume(TOKEN_RBRACE, "Expected '}' after if body");

        ASTNode* ifNode = (ASTNode*)arenaAlloc(arena, sizeof(ASTNode));
        ifNode->type = NODE_IF;
        ifNode->as.controlFlow.condition = conditionNode;
        ifNode->as.controlFlow.body = bodyNode;

        return ifNode;
    }

    if(currentToken.type == TOKEN_WHILE){
        advanceToken();

        consume(TOKEN_LPAREN, "Expected '(' after 'while'");
        ASTNode* conditionNode = parseEquality(arena);
        consume(TOKEN_RPAREN, "Expected ')' after condition");
        ASTNode* bodyNode = parseBlock(arena, table);

        ASTNode* whileNode = (ASTNode*)arenaAlloc(arena, sizeof(ASTNode));
        whileNode->type = NODE_WHILE;
        whileNode->as.controlFlow.condition = conditionNode;
        whileNode->as.controlFlow.body = bodyNode;

        return whileNode;
    }

    if(currentToken.type == TOKEN_IDENTIFIER){
        const char* varName = currentToken.start;
        int varLength = currentToken.length;

        advanceToken();

        if(currentToken.type == TOKEN_ASSIGN){
            advanceToken();
            if(getSymbolOffset(table, varName, varLength) == -1){
                addSymbol(table, varName, varLength);
            }

            ASTNode* exprNode = parseEquality(arena);

            consume(TOKEN_SEMICOLON, "Expected ';' after expression");

            ASTNode* assignNode = (ASTNode*)arenaAlloc(arena, sizeof(ASTNode));
            assignNode->type = NODE_ASSIGN;
            assignNode->as.assign.name = varName;
            assignNode->as.assign.length = varLength;
            assignNode->as.assign.expr = exprNode;

            return assignNode;
        }
    }

    return parseEquality(arena);
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
            if(node->as.binaryOp.operator == TOKEN_EQUAL_EQUAL){fprintf(stderr, "Operator: [==]\n");}
            else if(node->as.binaryOp.operator == TOKEN_PLUS) op = '+'; 
            else if(node->as.binaryOp.operator == TOKEN_MINUS) op = '-';
            else if(node->as.binaryOp.operator == TOKEN_STAR) op = '*';
            else if(node->as.binaryOp.operator == TOKEN_SLASH) op = '/';
            fprintf(stderr, "BinaryOp: [%c]\n", op);
            
            printAST(node->as.binaryOp.left, depth + 1);
            printAST(node->as.binaryOp.right, depth + 1);
            break;
        }
        case NODE_IF:
            fprintf(stderr, "If Statement:\n");
            printAST(node->as.controlFlow.condition, depth + 1);
            printAST(node->as.controlFlow.body, depth + 1);
            break;
        case NODE_WHILE:
            fprintf(stderr, "While Statement:\n");
            printAST(node->as.controlFlow.condition, depth + 1);
            printAST(node->as.controlFlow.body, depth + 1);
            break;
        case NODE_BLOCK:
            fprintf(stderr, "Block:\n");
            ASTNode* curr = node->as.block.head;
            while(curr != NULL) {
                printAST(curr, depth + 1);
                curr = curr->next;
            }
            break;
        case NODE_ASSIGN:
            fprintf(stderr, "Assign: %.*s\n", node->as.assign.length, node->as.assign.name);
            printAST(node->as.assign.expr, depth + 1);
            break;
        case NODE_PRINT:
            fprintf(stderr, "Print Statement:\n");
            printAST(node->as.print.expression, depth + 1); 
            break;
        default:
            fprintf(stderr, "Unknown node type\n");
    }
}