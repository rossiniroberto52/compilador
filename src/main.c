#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/mman.h> 
#include <sys/stat.h> 
#include <fcntl.h>   
#include <unistd.h>  

#include "arena.h"
#include "codegen.h"
#include "lexer.h"
#include "parser.h"
#include "arena.h"

char* mapFileToMem(const char* path, size_t* tamOut) {
    int fd = open(path, O_RDONLY);
    if(fd < 0) {
        fprintf(stderr, "Error: not possible to open file '%s'.\n", path);
        exit(74);
    }

    struct stat st;
    if(fstat(fd, &st) < 0) {
        fprintf(stderr, "Error: failed to read status of file\n");
        close(fd);
        exit(74);
    }

    *tamOut = st.st_size;

    char* buffer = mmap(NULL, *tamOut, PROT_READ, MAP_PRIVATE, fd, 0);

    if(buffer == MAP_FAILED) {
        fprintf(stderr, "Error: failed on mmap.\n");
        close(fd);
        exit(74);
    }

    close(fd);
    return buffer;
}


int main(int argc, const char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: compilator <path_to_source>\n");
        exit(64);
    }

    size_t fileSize;
    char* sourceCode = mapFileToMem(argv[1], &fileSize);

    initLexer(sourceCode, fileSize);
    
    Arena arena;
    initArena(&arena, 1024 * 1024); 

    advanceToken();

    ASTNode* root = parseExpression(&arena);

    fprintf(stderr, "--- ABSTRACT TREE ---\n");
    printAST(root, 0);
    fprintf(stderr, "--- ASSEMBLY ---\n");
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    generateAssembly(root);

    printf(" pop rax\n");
    printf("  ret\n");

    freeArena(&arena);
    munmap(sourceCode, fileSize);

    return 0;
}