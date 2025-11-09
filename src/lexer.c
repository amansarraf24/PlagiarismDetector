#include "lexer.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

static void add_token(TokenList *list, TokenType type, const char *value, int line) {
    if (!list || !value) return;
    
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->tokens = realloc(list->tokens, sizeof(Token) * list->capacity);
        if (!list->tokens) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
    }
    
    list->tokens[list->count].type = type;
    strncpy(list->tokens[list->count].value, value, 127);
    list->tokens[list->count].value[127] = '\0';
    list->tokens[list->count].line = line;
    list->count++;
}

TokenList* tokenize(const char *code) {
    if (!code) return NULL;
    
    TokenList *list = malloc(sizeof(TokenList));
    if (!list) return NULL;
    
    list->capacity = 1000;
    list->count = 0;
    list->tokens = malloc(sizeof(Token) * list->capacity);
    if (!list->tokens) {
        free(list);
        return NULL;
    }

    const char *ptr = code;
    int line = 1;

    while (*ptr != '\0') {
        if (isspace(*ptr)) {
            if (*ptr == '\n') line++;
            ptr++;
            continue;
        }

        if (*ptr == '/' && *(ptr+1) == '/') {
            while (*ptr && *ptr != '\n') ptr++;
            continue;
        }
        
        if (*ptr == '/' && *(ptr+1) == '*') {
            ptr += 2;
            while (*ptr && !(*ptr == '*' && *(ptr+1) == '/')) {
                if (*ptr == '\n') line++;
                ptr++;
            }
            if (*ptr) ptr += 2;
            continue;
        }

        if (*ptr == '#') {
            while (*ptr && *ptr != '\n') ptr++;
            continue;
        }

        if (isalpha(*ptr) || *ptr == '_') {
            char buffer[128];
            int i = 0;
            while ((isalnum(*ptr) || *ptr == '_') && i < 127) {
                buffer[i++] = *ptr++;
            }
            buffer[i] = '\0';
            TokenType type = is_keyword(buffer) ? TOK_KEYWORD : TOK_IDENTIFIER;
            add_token(list, type, buffer, line);
            continue;
        }

        if (isdigit(*ptr)) {
            char buffer[128];
            int i = 0;
            while ((isdigit(*ptr) || *ptr == '.') && i < 127) {
                buffer[i++] = *ptr++;
            }
            buffer[i] = '\0';
            add_token(list, TOK_NUMBER, buffer, line);
            continue;
        }

        char current[3] = {*ptr, '\0', '\0'};
        
        if (*(ptr+1) && strchr("=!<>&|+-", *ptr)) {
            if (((*ptr == '=' || *ptr == '!' || *ptr == '<' || *ptr == '>') && *(ptr+1) == '=') ||
                (*ptr == '&' && *(ptr+1) == '&') ||
                (*ptr == '|' && *(ptr+1) == '|') ||
                (*ptr == '+' && *(ptr+1) == '+') ||
                (*ptr == '-' && *(ptr+1) == '-')) {
                current[1] = *(ptr+1);
                ptr++;
            }
        }

        TokenType type = TOK_UNKNOWN;
        
        if (strchr("+-*/%<>!&|", current[0])) type = TOK_OPERATOR;
        else if (current[0] == '=') type = TOK_ASSIGN;
        else if (current[0] == ';') type = TOK_SEMICOLON;
        else if (current[0] == ',') type = TOK_COMMA;
        else if (current[0] == '(') type = TOK_LPAREN;
        else if (current[0] == ')') type = TOK_RPAREN;
        else if (current[0] == '{') type = TOK_LBRACE;
        else if (current[0] == '}') type = TOK_RBRACE;
        else if (current[0] == '[') type = TOK_LBRACKET;
        else if (current[0] == ']') type = TOK_RBRACKET;
        else if (current[0] == ':') type = TOK_COLON;
        
        if (type != TOK_UNKNOWN) {
            add_token(list, type, current, line);
        }
        
        ptr++;
    }

    add_token(list, TOK_EOF, "", line);
    return list;
}

void free_tokens(TokenList *list) {
    if (list) {
        if (list->tokens) free(list->tokens);
        free(list);
    }
}