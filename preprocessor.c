#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "preprocessor.h"

char* removeComments(const char* code) {
    if (code == NULL) {
        return NULL;
    }
    
    int len = strlen(code);
    char* result = (char*)malloc(len + 1);
    if (result == NULL) {
        printf("[ERROR] Memory allocation failed\n");
        return NULL;
    }
    
    int i = 0;
    int j = 0;
    int inSingleLineComment = 0;
    int inMultiLineComment = 0;
    int inString = 0;
    
    while (i < len) {
        if (inString) {
            result[j++] = code[i];
            if (code[i] == '"' && (i == 0 || code[i-1] != '\\')) {
                inString = 0;
            }
            i++;
            continue;
        }
        
        if (code[i] == '"' && (i == 0 || code[i-1] != '\\')) {
            inString = 1;
            result[j++] = code[i];
            i++;
            continue;
        }
        
        if (!inSingleLineComment && !inMultiLineComment) {
            if (i < len - 1 && code[i] == '/' && code[i+1] == '/') {
                inSingleLineComment = 1;
                i += 2;
                continue;
            }
            
            if (i < len - 1 && code[i] == '/' && code[i+1] == '*') {
                inMultiLineComment = 1;
                i += 2;
                continue;
            }
        }
        
        if (inSingleLineComment) {
            if (code[i] == '\n') {
                inSingleLineComment = 0;
                result[j++] = '\n';
            }
            i++;
            continue;
        }
        
        if (inMultiLineComment) {
            if (i < len - 1 && code[i] == '*' && code[i+1] == '/') {
                inMultiLineComment = 0;
                i += 2;
                continue;
            }
            i++;
            continue;
        }
        
        result[j++] = code[i++];
    }
    
    result[j] = '\0';
    return result;
}

char* normalizeWhitespace(const char* code) {
    if (code == NULL) {
        return NULL;
    }
    
    int len = strlen(code);
    char* result = (char*)malloc(len + 1);
    if (result == NULL) {
        printf("[ERROR] Memory allocation failed\n");
        return NULL;
    }
    
    int i = 0;
    int j = 0;
    int lastWasSpace = 0;
    int lastWasNewline = 0;
    
    while (i < len) {
        if (code[i] == '\t') {
            if (!lastWasSpace && !lastWasNewline) {
                result[j++] = ' ';
                lastWasSpace = 1;
            }
            i++;
            continue;
        }
        
        if (code[i] == ' ') {
            if (!lastWasSpace && !lastWasNewline) {
                result[j++] = ' ';
                lastWasSpace = 1;
            }
            i++;
            continue;
        }
        
        if (code[i] == '\n' || code[i] == '\r') {
            if (!lastWasNewline && j > 0) {
                result[j++] = '\n';
                lastWasNewline = 1;
                lastWasSpace = 0;
            }
            i++;
            continue;
        }
        
        result[j++] = code[i++];
        lastWasSpace = 0;
        lastWasNewline = 0;
    }
    
    result[j] = '\0';
    return result;
}

char* preprocessCode(const char* code) {
    if (code == NULL) {
        return NULL;
    }
    
    printf("[INFO] Preprocessing started...\n");
    
    char* noComments = removeComments(code);
    if (noComments == NULL) {
        printf("[ERROR] Comment removal failed\n");
        return NULL;
    }
    printf("  [OK] Comments removed\n");
    
    char* normalized = normalizeWhitespace(noComments);
    free(noComments);
    
    if (normalized == NULL) {
        printf("[ERROR] Whitespace normalization failed\n");
        return NULL;
    }
    printf("  [OK] Whitespace normalized\n");
    
    printf("[SUCCESS] Preprocessing completed\n");
    return normalized;
}

void printPreprocessedComparison(const char* original, const char* processed) {
    printf("\n========================================\n");
    printf("ORIGINAL CODE:\n");
    printf("========================================\n");
    printf("%s\n", original);
    
    printf("\n========================================\n");
    printf("PREPROCESSED CODE:\n");
    printf("========================================\n");
    printf("%s\n", processed);
    
    printf("\n========================================\n");
    printf("STATISTICS:\n");
    printf("========================================\n");
    printf("Original size: %lu bytes\n", (unsigned long)strlen(original));
    printf("Processed size: %lu bytes\n", (unsigned long)strlen(processed));
    printf("Reduction: %.1f%%\n", 
           100.0 * (1.0 - (double)strlen(processed) / strlen(original)));
    printf("========================================\n\n");
}