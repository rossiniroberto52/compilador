#include "codegen.h"
#include <stdio.h>

static int labelCount = 0;

void generateAssembly(ASTNode* node, SymbolTable* table) {
    if (node == NULL) return;

    if (node->type == NODE_NUMBER) {
        printf("  mov rax, %d\n", node->as.numberValue);
        printf("  push rax\n");
        return;
    }

    if(node -> type == NODE_IF){
        int currentLabel = labelCount++;
        generateAssembly(node->as.controlFlow.condition, table);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .L%d\n", currentLabel);
        generateAssembly(node->as.controlFlow.body, table);
        printf(".L%d:\n", currentLabel);
        return;
    }

    if (node->type == NODE_WHILE) {
        int labelStart = labelCount++;
        int labelEnd = labelCount++;
        printf(".L%d:\n", labelStart);
        generateAssembly(node->as.controlFlow.condition, table);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .L%d\n", labelEnd);
        generateAssembly(node->as.controlFlow.body, table);
        printf("  jmp .L%d\n", labelStart);
        printf(".L%d:\n", labelEnd);
        return;
    }

    if (node->type == NODE_BLOCK){
        ASTNode* current = node->as.block.head;
        while(current != NULL){
            generateAssembly(current, table);
            current = current->next;
        }
        return;
    }

    if(node->type == NODE_IDENTIFIER){
        //int offset = getSymbolOffset(table, node->as.identifier.name, node->as.identifier.length);
        if(node->as.identifier.offset == -1){
            fprintf(stderr, "Error: Variable '%.*s' not declared\n", node->as.identifier.length, node->as.identifier.name);
            return;
        }
        printf("  mov rax, [rbp - %d]\n", node->as.identifier.offset);
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
        //int offset = getSymbolOffset(table, node->as.assign.name, node->as.assign.length);
        printf("  pop rax\n");
        printf("  mov [rbp - %d], rax\n", node->as.assign.offset);
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
        }else if(node->as.binaryOp.operator == TOKEN_EQUAL_EQUAL){
            printf("  cmp rax, rbx\n");
            printf("  sete al\n");
            printf("  movzx rax, al\n");
        }else if(node->as.binaryOp.operator == TOKEN_LESS){
            printf("  cmp rax, rbx\n");
            printf("  setl al\n");
            printf("  movzx rax, al\n");
        }else if(node->as.binaryOp.operator == TOKEN_LESS_EQUAL){
            printf("  cmp rax, rbx\n");
            printf("  setle al\n");
            printf("  movzx rax, al\n");
        }else if(node->as.binaryOp.operator == TOKEN_GREATER){
            printf("  cmp rax, rbx\n");
            printf("  setg al\n");
            printf("  movzx rax, al\n");
        }else if(node->as.binaryOp.operator == TOKEN_GREATER_EQUAL){
            printf("  cmp rax, rbx\n");
            printf("  setge al\n");
            printf("  movzx rax, al\n");
        }else if(node->as.binaryOp.operator == TOKEN_BANG_EQUAL){
            printf("  cmp rax, rbx\n");
            printf("  setne al\n");
            printf("  movzx rax, al\n");
        }
        
        printf("  push rax\n");
        return;
    }

    if (node->type == NODE_LOGICAL_AND) {
        int labelFalse = labelCount++;
        int labelEnd = labelCount++;

        generateAssembly(node->as.binaryOp.left, table);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .L%d\n", labelFalse);

        generateAssembly(node->as.binaryOp.right, table);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .L%d\n", labelFalse);

        printf("  mov rax, 1\n");
        printf("  jmp .L%d\n", labelEnd);

        printf(".L%d:\n", labelFalse);
        printf("  mov rax, 0\n");

        printf(".L%d:\n", labelEnd);
        printf("  push rax\n");
        return;
    }

    if (node->type == NODE_LOGICAL_OR) {
        int labelTrue = labelCount++;
        int labelEnd = labelCount++;

        generateAssembly(node->as.binaryOp.left, table);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  jne .L%d\n", labelTrue);

        generateAssembly(node->as.binaryOp.right, table);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  jne .L%d\n", labelTrue);

        printf("  mov rax, 0\n");
        printf("  jmp .L%d\n", labelEnd);

        printf(".L%d:\n", labelTrue);
        printf("  mov rax, 1\n");

        printf(".L%d:\n", labelEnd);
        printf("  push rax\n");
        return;
    }
}