#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ngram_generator.h"

unsigned long hashString(const char* str) {
    unsigned long hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    
    return hash;
}

TokenList* tokenize(const char* code) {
    if (code == NULL) {
        return NULL;
    }
    
    TokenList* list = (TokenList*)malloc(sizeof(TokenList));
    if (list == NULL) {
        printf("[ERROR] Memory allocation failed\n");
        return NULL;
    }
    
    list->count = 0;
    
    int i = 0;
    int len = strlen(code);
    
    while (i < len && list->count < MAX_TOKENS) {
        while (i < len && isspace(code[i])) {
            i++;
        }
        
        if (i >= len) break;
        
        int tokenStart = i;
        
        if (isalpha(code[i]) || code[i] == '_') {
            while (i < len && (isalnum(code[i]) || code[i] == '_')) {
                i++;
            }
        }
        else if (isdigit(code[i])) {
            while (i < len && (isdigit(code[i]) || code[i] == '.')) {
                i++;
            }
        }
        else {
            i++;
        }
        
        int tokenLen = i - tokenStart;
        if (tokenLen > 0 && tokenLen < MAX_TOKEN_LENGTH) {
            strncpy(list->tokens[list->count], &code[tokenStart], tokenLen);
            list->tokens[list->count][tokenLen] = '\0';
            list->count++;
        }
    }
    
    return list;
}

NgramList* generateNgrams(TokenList* tokens, int n) {
    if (tokens == NULL || n <= 0 || n > tokens->count) {
        return NULL;
    }
    
    NgramList* list = (NgramList*)malloc(sizeof(NgramList));
    if (list == NULL) {
        printf("[ERROR] Memory allocation failed\n");
        return NULL;
    }
    
    list->count = 0;
    
    for (int i = 0; i <= tokens->count - n && list->count < MAX_NGRAMS; i++) {
        char ngram[MAX_TOKEN_LENGTH * DEFAULT_N] = "";
        
        for (int j = 0; j < n; j++) {
            strcat(ngram, tokens->tokens[i + j]);
            if (j < n - 1) {
                strcat(ngram, " ");
            }
        }
        
        strcpy(list->ngrams[list->count], ngram);
        list->hashes[list->count] = hashString(ngram);
        list->count++;
    }
    
    return list;
}

void printTokens(TokenList* tokens, int limit) {
    if (tokens == NULL) {
        printf("[ERROR] Token list is NULL\n");
        return;
    }
    
    printf("\n========================================\n");
    printf("TOKENS: %d total\n", tokens->count);
    printf("========================================\n");
    
    int displayCount = (limit > 0 && limit < tokens->count) ? limit : tokens->count;
    
    for (int i = 0; i < displayCount; i++) {
        printf("[%d] %s\n", i + 1, tokens->tokens[i]);
    }
    
    if (limit > 0 && limit < tokens->count) {
        printf("... (%d more tokens)\n", tokens->count - limit);
    }
    
    printf("========================================\n\n");
}

void printNgrams(NgramList* ngrams, int limit) {
    if (ngrams == NULL) {
        printf("[ERROR] N-gram list is NULL\n");
        return;
    }
    
    printf("\n========================================\n");
    printf("N-GRAMS: %d total\n", ngrams->count);
    printf("========================================\n");
    
    int displayCount = (limit > 0 && limit < ngrams->count) ? limit : ngrams->count;
    
    for (int i = 0; i < displayCount; i++) {
        printf("[%d] \"%s\"\n", i + 1, ngrams->ngrams[i]);
        printf("     Hash: %lu\n", ngrams->hashes[i]);
    }
    
    if (limit > 0 && limit < ngrams->count) {
        printf("... (%d more n-grams)\n", ngrams->count - limit);
    }
    
    printf("========================================\n\n");
}

void freeTokenList(TokenList* list) {
    if (list != NULL) {
        free(list);
    }
}

void freeNgramList(NgramList* list) {
    if (list != NULL) {
        free(list);
    }
}