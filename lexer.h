#ifndef LEXER_H
#define LEXER_H

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

typedef struct {
    const char* start;   
    const char* current; 
    const char* end;     
    int line;            
} Lexer;

extern Token currentToken;
extern Token previousToken; 
extern Lexer lexer;

void initLexer(const char* source, size_t length);
Token scanToken();
void advanceToken();
void consume(TokenType type, const char* message);

#endif