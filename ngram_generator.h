#ifndef NGRAM_GENERATOR_H
#define NGRAM_GENERATOR_H

#define MAX_TOKENS 10000
#define MAX_TOKEN_LENGTH 256
#define MAX_NGRAMS 10000
#define DEFAULT_N 3

typedef struct {
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int count;
} TokenList;

typedef struct {
    char ngrams[MAX_NGRAMS][MAX_TOKEN_LENGTH * DEFAULT_N];
    unsigned long hashes[MAX_NGRAMS];
    int count;
} NgramList;

TokenList* tokenize(const char* code);
NgramList* generateNgrams(TokenList* tokens, int n);
unsigned long hashString(const char* str);
void freeTokenList(TokenList* list);
void freeNgramList(NgramList* list);
void printTokens(TokenList* tokens, int limit);
void printNgrams(NgramList* ngrams, int limit);

#endif