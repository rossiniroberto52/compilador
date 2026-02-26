#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/mman.h> 
#include <sys/stat.h> 
#include <fcntl.h>   
#include <unistd.h>  

typedef enum {
    TOKEN_IDENTIFIER, TOKEN_NUMBER,
    TOKEN_ASSIGN,     
    TOKEN_PLUS,       
    TOKEN_MINUS,      
    TOKEN_STAR,       
    TOKEN_SLASH,      
    TOKEN_LPAREN,     
    TOKEN_RPAREN,     
    TOKEN_LBRACE,    
    TOKEN_RBRACE,     
    TOKEN_SEMICOLON,  
    TOKEN_IF, TOKEN_WHILE, TOKEN_PRINT,
    TOKEN_EOF,        
    TOKEN_ERROR       
} TokenType;

typedef struct {
    TokenType type;
    const char* start;
    int length;
    int line;
} Token;

Token currentToken;
Token previusToken;

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
            int lenght;
        } identifier;

        struct {
            struct ASTNode* left;
            struct ASTNode* right;
            TokenType operator;
        } binaryOp;

        struct {
            const char* name;
            int lenght;
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

typedef struct{
    char* memory;
    size_t capacity;
    size_t offset;
} Arena;

typedef struct {
    const char* start;   
    const char* current; 
    const char* end;     
    int line;            
} Lexer;

Lexer lexer; 

void initLexer(const char* source, size_t length) {
    lexer.start = source;
    lexer.current = source;
    lexer.end = source + length;
    lexer.line = 1;
}

static int isAtEnd() {
    return lexer.current >= lexer.end; 
}

static char advance() {
    return *lexer.current++;
}

static char peek() {
    if(isAtEnd()) return '\0';
    return *lexer.current;
}

static int isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static int isDigit(char c) {
    return c >= '0' && c <= '9';
}

static Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = lexer.start;
    token.length = (int)(lexer.current - lexer.start); 
    token.line = lexer.line;
    return token;
}

static void advanceToken(){
    previusToken = currentToken;
    for(;;){
        currentToken = scanToken();
        if (currentToken.type != TOKEN_ERROR) break;

        fprintf(stderr, "Error: Unexpected character '%.*s' at line %d.\n", currentToken.length, currentToken.start, currentToken.line);
        exit(65);
    }
}

static void consume(TokenType type, const char* message){
    if(currentToken.type == type){
        advanceToken();
        return;
    }
    fprintf(stderr, "Error: %s at line %d.\n", message, currentToken.line);
    exit(65);
}

static ASTNode* parseTerm(Arena* arena);
static ASTNode* parseFactor(Arena* arena);

static ASTNode* parseExpression(Arena* arena){
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

static void skipSpace() {
    for(;;) {
        char c = peek(); 
        switch(c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                lexer.line++;
                advance(); 
                break;
            default:
                return;
        }
    }
}

static TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
    int tokenLength = (int)(lexer.current - lexer.start);
    if (tokenLength == start + length && 
        strncmp(lexer.start + start, rest, length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}

static TokenType identifierType() {
    switch (lexer.start[0]) {
        case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
        case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
        case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
    }
    return TOKEN_IDENTIFIER;
}

Token scanToken() {
    skipSpace();
    lexer.start = lexer.current;

    if(isAtEnd()) return makeToken(TOKEN_EOF);

    char c = advance();

    switch (c) {
        case '(': return makeToken(TOKEN_LPAREN);
        case ')': return makeToken(TOKEN_RPAREN);
        case '{': return makeToken(TOKEN_LBRACE);
        case '}': return makeToken(TOKEN_RBRACE);
        case ';': return makeToken(TOKEN_SEMICOLON);
        case '+': return makeToken(TOKEN_PLUS);
        case '-': return makeToken(TOKEN_MINUS);
        case '*': return makeToken(TOKEN_STAR);
        case '/': return makeToken(TOKEN_SLASH);
        case '=': return makeToken(TOKEN_ASSIGN);
    }

    if(isDigit(c)) {
        while (isDigit(peek())) advance();
        return makeToken(TOKEN_NUMBER);
    }

    if(isAlpha(c)) {
        while(isAlpha(peek()) || isDigit(peek())) advance();
        return makeToken(identifierType());
    }

    return makeToken(TOKEN_ERROR);
}

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

void initArena(Arena* arena, size_t capacity){
    arena->memory = (char*)malloc(capacity);
    if(arena->memory == NULL){
        fprintf(stderr, "Error: Failed to allocate mem arena.\n");
        exit(74);
    }
    arena->capacity = capacity;
    arena->offset = 0;
}

void* arenaAlloc(Arena* arena, size_t size){
    if(arena->offset+size > arena->capacity){
        fprintf(stderr, "Error: Memory overflow");
        exit(74);
    }

    size = (size+7) & ~7;

    void* mem = arena->memory + arena->offset;
    arena->offset += size;

    return mem;
}

void freeArena(Arena* arena){
    free(arena->memory);
    arena->memory = NULL;
    arena->offset = 0;
    arena->capacity = 0;
}

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: compilador <path_to_source>\n");
        exit(64);
    }

    size_t fileSize;
    char* sourceCode = mapFileToMem(argv[1], &fileSize);

    initLexer(sourceCode, fileSize);

    int line = -1;
    for (;;) {
        Token token = scanToken();

        if (token.line != line) {
            printf("%4d ", token.line);
            line = token.line;
        } else {
            printf("   | ");
        }

        printf("%2d '%.*s'\n", token.type, token.length, token.start);

        if (token.type == TOKEN_EOF) break;
    }

    munmap(sourceCode, fileSize);

    return 0;
}