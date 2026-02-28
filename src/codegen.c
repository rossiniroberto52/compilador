#include "codegen.h"
#include <stdio.h>

void generateAssembly(ASTNode* node) {
    if (node == NULL) return;

    if (node->type == NODE_NUMBER) {
        printf("  mov rax, %d\n", node->as.numberValue);
        printf("  push rax\n");
        return;
    }

    if (node->type == NODE_BINARY_OP) {
        generateAssembly(node->as.binaryOp.left);
        generateAssembly(node->as.binaryOp.right);

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