#ifndef SIMILARITY_CALCULATOR_H
#define SIMILARITY_CALCULATOR_H

#include "ngram_generator.h"
#include "directory_handler.h"

typedef struct {
    char file1[256];
    char file2[256];
    int matchingNgrams;
    int totalNgrams;
    float similarityPercentage;
} ComparisonResult;

typedef struct {
    ComparisonResult results[100];
    int count;
} ComparisonReport;

ComparisonResult compareNgrams(NgramList* ngrams1, NgramList* ngrams2, 
                               const char* file1, const char* file2);
ComparisonReport* compareAllFiles(FileList* files, int ngramSize);
void printComparisonResult(ComparisonResult* result);
void printComparisonReport(ComparisonReport* report, float threshold);
void freeComparisonReport(ComparisonReport* report);
int countMatches(NgramList* ngrams1, NgramList* ngrams2);

#endif