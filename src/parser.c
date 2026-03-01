#include "parser.h"
#include "symbol.h"
#include <stdio.h>
#include <stdlib.h>

extern Token currentToken;
extern void advanceToken();
extern void consume(TokenType type, const char* message);

ASTNode* parseBlock(Arena* arena, SymbolTable* table);
ASTNode* parseStatement(Arena* arena, SymbolTable* table);
ASTNode* parseLogicalOr(Arena* arena, SymbolTable* table);
ASTNode* parseLogicalAnd(Arena* arena, SymbolTable* table);
ASTNode* parseEquality(Arena* arena, SymbolTable* table);
ASTNode* parseExpression(Arena* arena, SymbolTable* table);
static ASTNode* parseTerm(Arena* arena, SymbolTable* table);
static ASTNode* parseFactor(Arena* arena, SymbolTable* table);

static ASTNode* parseFactor(Arena* arena, SymbolTable* table){
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
        node->as.identifier.offset = getSymbolOffset(table, currentToken.start, currentToken.length);

        advanceToken();
        return node;
    }

    if(currentToken.type == TOKEN_LPAREN){
        advanceToken();
        ASTNode* node = parseLogicalOr(arena, table);
        consume(TOKEN_RPAREN, "Expected ')' after expression");
        return node;
    }

    fprintf(stderr, "Error: Expected expression at line %d.\n", currentToken.line);
    exit(65);
}

static ASTNode* parseTerm(Arena* arena, SymbolTable* table){
    ASTNode* node = parseFactor(arena, table);

    while(currentToken.type == TOKEN_STAR || currentToken.type == TOKEN_SLASH){
        TokenType operator = currentToken.type;
        advanceToken();

        ASTNode* rigthtNode = parseFactor(arena, table);

        ASTNode* binaryNode = (ASTNode*)arenaAlloc(arena, sizeof(ASTNode));
        
        binaryNode->type = NODE_BINARY_OP;
        binaryNode->as.binaryOp.left = node;
        binaryNode->as.binaryOp.right = rigthtNode;
        binaryNode->as.binaryOp.operator = operator;

        node = binaryNode;
    }

    return node;
}

ASTNode* parseExpression(Arena* arena, SymbolTable* table){
    ASTNode* node = parseTerm(arena, table);
    while(currentToken.type == TOKEN_PLUS || currentToken.type == TOKEN_MINUS){
        TokenType  operator =  currentToken.type;
        advanceToken();

        ASTNode* rightnode = parseTerm(arena, table);

        ASTNode* binaryNode = (ASTNode*)arenaAlloc(arena, sizeof(ASTNode));

        binaryNode->type = NODE_BINARY_OP;
        binaryNode->as.binaryOp.left = node;
        binaryNode->as.binaryOp.right = rightnode;
        binaryNode->as.binaryOp.operator = operator;
        node = binaryNode;
    }
    return node;
}

ASTNode* parseEquality(Arena* arena, SymbolTable* table){
    ASTNode* left = parseExpression(arena, table);

    while(currentToken.type == TOKEN_EQUAL_EQUAL || 
          currentToken.type == TOKEN_BANG_EQUAL ||
          currentToken.type == TOKEN_LESS ||
          currentToken.type == TOKEN_LESS_EQUAL ||
          currentToken.type == TOKEN_GREATER ||
          currentToken.type == TOKEN_GREATER_EQUAL) {
        
        TokenType operatorType = currentToken.type;
        advanceToken();

        ASTNode* right = parseExpression(arena, table);

        ASTNode* node = (ASTNode*)arenaAlloc(arena, sizeof(ASTNode));
        node->type = NODE_BINARY_OP;
        node->as.binaryOp.operator = operatorType;
        node->as.binaryOp.left = left;
        node->as.binaryOp.right = right;

        left = node;
    }

    return left;
}

ASTNode* parseLogicalAnd(Arena* arena, SymbolTable* table){
    ASTNode* left = parseEquality(arena, table);
    while(currentToken.type == TOKEN_LOGICAL_AND){
        advanceToken();
        ASTNode* right = parseEquality(arena, table);

        ASTNode* node = (ASTNode*)arenaAlloc(arena, sizeof(ASTNode));
        node->type = NODE_LOGICAL_AND;
        node->as.binaryOp.left = left;
        node->as.binaryOp.right = right;
        left = node;
    }
    return left; 
}

ASTNode* parseLogicalOr(Arena* arena, SymbolTable* table){
    ASTNode* left = parseLogicalAnd(arena, table);
    while(currentToken.type == TOKEN_LOGICAL_OR){
        advanceToken();
        ASTNode* right = parseLogicalAnd(arena, table);

        ASTNode* node = (ASTNode*)arenaAlloc(arena, sizeof(ASTNode));
        node->type = NODE_LOGICAL_OR;
        node->as.binaryOp.left = left;
        node->as.binaryOp.right = right;
        left = node;
    }
    return left;
}

ASTNode* parseBlock(Arena* arena, SymbolTable* table){
    consume(TOKEN_LBRACE, "Expected '{' at the beginning of block");

    beginScope(table);

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

    endScope(table);

    return blockNode;
}

ASTNode* parseStatement(Arena* arena, SymbolTable* table){
    if(currentToken.type == TOKEN_PRINT){
        advanceToken();
        consume(TOKEN_LPAREN, "Expected '(' after 'print'");
        ASTNode* exprNode = parseLogicalOr(arena, table);
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
        ASTNode* conditionNode = parseLogicalOr(arena, table);
        consume(TOKEN_RPAREN, "Expected ')' after condition");

        ASTNode* bodyNode = parseBlock(arena, table);

        //consume(TOKEN_LBRACE, "Expected '{' before if body");
        //ASTNode* bodyNode = parseStatement(arena, table);
        //consume(TOKEN_RBRACE, "Expected '}' after if body");

        ASTNode* ifNode = (ASTNode*)arenaAlloc(arena, sizeof(ASTNode));
        ifNode->type = NODE_IF;
        ifNode->as.controlFlow.condition = conditionNode;
        ifNode->as.controlFlow.body = bodyNode;

        return ifNode;
    }

    if(currentToken.type == TOKEN_WHILE){
        advanceToken();

        consume(TOKEN_LPAREN, "Expected '(' after 'while'");
        ASTNode* conditionNode = parseLogicalOr(arena, table); 
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

            ASTNode* exprNode = parseLogicalOr(arena, table);

            consume(TOKEN_SEMICOLON, "Expected ';' after expression");

            ASTNode* assignNode = (ASTNode*)arenaAlloc(arena, sizeof(ASTNode));
            assignNode->type = NODE_ASSIGN;
            assignNode->as.assign.name = varName;
            assignNode->as.assign.length = varLength;
            assignNode->as.assign.expr = exprNode;
            assignNode->as.assign.offset = getSymbolOffset(table, varName, varLength);

            return assignNode;
        }
    }

    return parseLogicalOr(arena, table);
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
            const char* opStr = "?";
            if(node->as.binaryOp.operator == TOKEN_EQUAL_EQUAL) opStr = "==";
            else if(node->as.binaryOp.operator == TOKEN_BANG_EQUAL) opStr = "!=";
            else if(node->as.binaryOp.operator == TOKEN_LESS) opStr = "<";
            else if(node->as.binaryOp.operator == TOKEN_LESS_EQUAL) opStr = "<=";
            else if(node->as.binaryOp.operator == TOKEN_GREATER) opStr = ">";
            else if(node->as.binaryOp.operator == TOKEN_GREATER_EQUAL) opStr = ">=";
            else if(node->as.binaryOp.operator == TOKEN_PLUS) opStr = "+"; 
            else if(node->as.binaryOp.operator == TOKEN_MINUS) opStr = "-";
            else if(node->as.binaryOp.operator == TOKEN_STAR) opStr = "*";
            else if(node->as.binaryOp.operator == TOKEN_SLASH) opStr = "/";
            
            fprintf(stderr, "BinaryOp: [%s]\n", opStr);
            printAST(node->as.binaryOp.left, depth + 1);
            printAST(node->as.binaryOp.right, depth + 1);
            break;
        }
        case NODE_LOGICAL_AND:
            fprintf(stderr, "Logical AND [&&]\n");
            printAST(node->as.binaryOp.left, depth + 1);
            printAST(node->as.binaryOp.right, depth + 1);
            break;
        case NODE_LOGICAL_OR: 
            fprintf(stderr, "Logical OR [||]\n");
            printAST(node->as.binaryOp.left, depth + 1);
            printAST(node->as.binaryOp.right, depth + 1);
            break;
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