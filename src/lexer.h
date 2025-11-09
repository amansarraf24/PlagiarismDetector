#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOK_KEYWORD,
    TOK_IDENTIFIER,
    TOK_NUMBER,
    TOK_OPERATOR,
    TOK_ASSIGN,
    TOK_SEMICOLON,
    TOK_COMMA,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_COLON,
    TOK_EOF,
    TOK_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char value[128];
    int line;
} Token;

typedef struct {
    Token *tokens;
    int count;
    int capacity;
} TokenList;

TokenList* tokenize(const char *code);
void free_tokens(TokenList *list);

#endif