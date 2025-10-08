#include <stdio.h>
#include <stdlib.h>
#include "file_handler.h"
#include "directory_handler.h"
#include "preprocessor.h"
#include "ngram_generator.h"
#include "similarity_calculator.h"

int main() {
    printf("\n========================================\n");
    printf("  PLAGIARISM DETECTOR - MODULE 5\n");
    printf("  Similarity Calculator\n");
    printf("========================================\n\n");
    
    const char* folderPath = "../test_files";
    int ngramSize = 3;
    float threshold = 50.0f;
    
    printf("[CONFIG]\n");
    printf("  Directory: %s\n", folderPath);
    printf("  N-gram size: %d\n", ngramSize);
    printf("  Plagiarism threshold: %.1f%%\n\n", threshold);
    
    printf("[INFO] Scanning directory...\n\n");
    
    FileList* files = scanDirectory(folderPath);
    
    if (files == NULL || files->count == 0) {
        printf("[ERROR] No .c files found!\n");
        if (files) freeFileList(files);
        return 1;
    }
    
    printFileList(files);
    
    if (files->count < 2) {
        printf("[ERROR] Need at least 2 files for comparison!\n");
        freeFileList(files);
        return 1;
    }
    
    printf("[INFO] Starting plagiarism detection...\n");
    printf("========================================\n\n");
    
    ComparisonReport* report = compareAllFiles(files, ngramSize);
    
    if (report == NULL) {
        printf("[ERROR] Comparison failed!\n");
        freeFileList(files);
        return 1;
    }
    
    printf("\n[SUCCESS] Analysis completed!\n");
    
    printComparisonReport(report, threshold);
    
    freeComparisonReport(report);
    freeFileList(files);
    
    printf("========================================\n");
    printf("[DONE] Plagiarism detection completed!\n");
    printf("========================================\n\n");
    
    return 0;
}