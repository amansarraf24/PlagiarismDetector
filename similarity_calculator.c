#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "similarity_calculator.h"
#include "file_handler.h"
#include "preprocessor.h"

int countMatches(NgramList* ngrams1, NgramList* ngrams2) {
    if (ngrams1 == NULL || ngrams2 == NULL) {
        return 0;
    }
    
    int matches = 0;
    
    for (int i = 0; i < ngrams1->count; i++) {
        for (int j = 0; j < ngrams2->count; j++) {
            if (ngrams1->hashes[i] == ngrams2->hashes[j]) {
                matches++;
                break;
            }
        }
    }
    
    return matches;
}

ComparisonResult compareNgrams(NgramList* ngrams1, NgramList* ngrams2, 
                               const char* file1, const char* file2) {
    ComparisonResult result;
    
    strncpy(result.file1, file1, 255);
    result.file1[255] = '\0';
    strncpy(result.file2, file2, 255);
    result.file2[255] = '\0';
    
    result.matchingNgrams = countMatches(ngrams1, ngrams2);
    result.totalNgrams = ngrams1->count;
    
    if (result.totalNgrams > 0) {
        result.similarityPercentage = 
            (float)result.matchingNgrams / result.totalNgrams * 100.0f;
    } else {
        result.similarityPercentage = 0.0f;
    }
    
    return result;
}

ComparisonReport* compareAllFiles(FileList* files, int ngramSize) {
    if (files == NULL || files->count < 2) {
        printf("[ERROR] Need at least 2 files for comparison\n");
        return NULL;
    }
    
    ComparisonReport* report = (ComparisonReport*)malloc(sizeof(ComparisonReport));
    if (report == NULL) {
        printf("[ERROR] Memory allocation failed\n");
        return NULL;
    }
    
    report->count = 0;
    
    NgramList* allNgrams[files->count];
    
    printf("[INFO] Preprocessing all files...\n\n");
    
    for (int i = 0; i < files->count; i++) {
        printf("  Processing: %s\n", files->paths[i]);
        
        char* content = readFile(files->paths[i]);
        if (content == NULL) {
            allNgrams[i] = NULL;
            continue;
        }
        
        char* preprocessed = preprocessCode(content);
        free(content);
        
        if (preprocessed == NULL) {
            allNgrams[i] = NULL;
            continue;
        }
        
        TokenList* tokens = tokenize(preprocessed);
        free(preprocessed);
        
        if (tokens == NULL) {
            allNgrams[i] = NULL;
            continue;
        }
        
        allNgrams[i] = generateNgrams(tokens, ngramSize);
        freeTokenList(tokens);
        
        if (allNgrams[i] != NULL) {
            printf("    [OK] %d n-grams generated\n", allNgrams[i]->count);
        } else {
            printf("    [FAIL] N-gram generation failed\n");
        }
    }
    
    printf("\n[INFO] Comparing files...\n\n");
    
    for (int i = 0; i < files->count - 1; i++) {
        if (allNgrams[i] == NULL) continue;
        
        for (int j = i + 1; j < files->count; j++) {
            if (allNgrams[j] == NULL) continue;
            
            ComparisonResult result = compareNgrams(
                allNgrams[i], 
                allNgrams[j],
                files->paths[i],
                files->paths[j]
            );
            
            report->results[report->count++] = result;
            
            printf("  [%s] vs [%s] = %.1f%%\n", 
                   files->paths[i], 
                   files->paths[j], 
                   result.similarityPercentage);
        }
    }
    
    for (int i = 0; i < files->count; i++) {
        if (allNgrams[i] != NULL) {
            freeNgramList(allNgrams[i]);
        }
    }
    
    return report;
}

void printComparisonResult(ComparisonResult* result) {
    printf("\n========================================\n");
    printf("COMPARISON RESULT\n");
    printf("========================================\n");
    printf("File 1: %s\n", result->file1);
    printf("File 2: %s\n", result->file2);
    printf("----------------------------------------\n");
    printf("Matching N-grams: %d / %d\n", 
           result->matchingNgrams, result->totalNgrams);
    printf("Similarity: %.2f%%\n", result->similarityPercentage);
    
    if (result->similarityPercentage >= 80.0f) {
        printf("Status: HIGH PLAGIARISM DETECTED!\n");
    } else if (result->similarityPercentage >= 50.0f) {
        printf("Status: MODERATE SIMILARITY\n");
    } else {
        printf("Status: LOW SIMILARITY\n");
    }
    
    printf("========================================\n");
}

void printComparisonReport(ComparisonReport* report, float threshold) {
    if (report == NULL) {
        printf("[ERROR] Report is NULL\n");
        return;
    }
    
    printf("\n========================================\n");
    printf("PLAGIARISM DETECTION REPORT\n");
    printf("========================================\n");
    printf("Total Comparisons: %d\n", report->count);
    printf("Threshold: %.1f%%\n", threshold);
    printf("========================================\n\n");
    
    int flaggedCount = 0;
    
    for (int i = 0; i < report->count; i++) {
        if (report->results[i].similarityPercentage >= threshold) {
            flaggedCount++;
            
            printf(" FLAGGED PAIR #%d:\n", flaggedCount);
            printf(" File 1: %s\n", report->results[i].file1);
            printf(" File 2: %s\n", report->results[i].file2);
            printf(" Similarity: %.2f%%\n", report->results[i].similarityPercentage);
            printf(" Matches: %d / %d n-grams\n\n",
                   report->results[i].matchingNgrams,
                   report->results[i].totalNgrams);
        }
    }
    
    if (flaggedCount == 0) {
        printf(" No plagiarism detected above threshold!\n\n");
    } else {
        printf("========================================\n");
        printf("  TOTAL FLAGGED: %d pairs\n", flaggedCount);
        printf("========================================\n\n");
    }
    
    printf("DETAILED RESULTS:\n");
    printf("========================================\n");
    for (int i = 0; i < report->count; i++) {
        printf("[%d] %.1f%% | %s vs %s\n",
               i + 1,
               report->results[i].similarityPercentage,
               report->results[i].file1,
               report->results[i].file2);
    }
    printf("========================================\n\n");
}

void freeComparisonReport(ComparisonReport* report) {
    if (report != NULL) {
        free(report);
    }
}