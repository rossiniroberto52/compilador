#include "lexer.h"

Token currentToken;
Token previousToken;
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

#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

extern Token currentToken;
extern void advanceToken();
extern void consume(TokenType type, const char* message);

void skipSpace() {
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

static Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = lexer.start;
    token.length = (int)(lexer.current - lexer.start); 
    token.line = lexer.line;
    return token;
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

void advanceToken(){
    previousToken = currentToken;
    for(;;){
        currentToken = scanToken();
        if (currentToken.type != TOKEN_ERROR) break;

        fprintf(stderr, "Error: Unexpected character '%.*s' at line %d.\n", currentToken.length, currentToken.start, currentToken.line);
        exit(65);
    }
}

void consume(TokenType type, const char* message){
    if(currentToken.type == type){
        advanceToken();
        return;
    }
    fprintf(stderr, "Error: %s at line %d.\n", message, currentToken.line);
    exit(65);
}



