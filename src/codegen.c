#include "codegen.h"
#include <stdio.h>

void generateAssembly(ASTNode* node, SymbolTable* table) {
    if (node == NULL) return;

    if (node->type == NODE_NUMBER) {
        printf("  mov rax, %d\n", node->as.numberValue);
        printf("  push rax\n");
        return;
    }

    if(node->type == NODE_IDENTIFIER){
        int offset = getSymbolOffset(table, node->as.identifier.name, node->as.identifier.length);
        printf("  mov rax, [rbp - %d]\n", offset);
        printf("  push rax\n");
        return;
    }

    if(node->type == NODE_PRINT){
        generateAssembly(node->as.print.expression, table);
        printf("  pop rsi\n\n");
        printf("  lea rdi, [rip + .LC0]\n");
        printf("  mov rax, 0\n");
        printf("  call printf@PLT\n");
        return;
    }

    if(node->type == NODE_ASSIGN){
        generateAssembly(node->as.assign.expr, table);
        int offset = getSymbolOffset(table, node->as.assign.name, node->as.assign.length);
        printf("  pop rax\n");
        printf("  mov [rbp - %d], rax\n", offset);
        return;
    }

    if (node->type == NODE_BINARY_OP) {
        generateAssembly(node->as.binaryOp.left, table);
        generateAssembly(node->as.binaryOp.right, table);

        printf("  pop rbx\n"); 
        printf("  pop rax\n"); 

        if (node->as.binaryOp.operator == TOKEN_PLUS) {
            printf("  add rax, rbx\n");
        } 
        else if (node->as.binaryOp.operator == TOKEN_MINUS) {
            printf("  sub rax, rbx\n");
        }
        else if (node->as.binaryOp.operator == TOKEN_STAR) {
            printf("  imul rax, rbx\n");
        }
        else if (node->as.binaryOp.operator == TOKEN_SLASH) {
            printf("  cqo\n");
            printf("  idiv rbx\n");
        }
        
        printf("  push rax\n");
        return;
    }
}