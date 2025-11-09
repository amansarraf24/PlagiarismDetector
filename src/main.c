#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "directory_handler.h"
#include "file_handler.h"
#include "detector.h"

void print_separator() {
    printf("================================================================\n");
}

void print_result(const char *file1, const char *file2, PlagiarismResult result) {
    print_separator();
    printf("Comparing:\n");
    printf("  File 1: %s (%d nodes)\n", file1, result.total_nodes_1);
    printf("  File 2: %s (%d nodes)\n", file2, result.total_nodes_2);
    printf("\n");
    printf("Similarity Metrics:\n");
    printf("  AST Similarity:   %.2f%%\n", result.ast_similarity * 100);
    printf("  CFG Similarity:   %.2f%%\n", result.cfg_similarity * 100);
    printf("  DAG Similarity:   %.2f%%\n", result.dag_similarity * 100);
    printf("\n");
    printf("OVERALL SCORE:    %.2f%%\n", result.overall_score * 100);
    printf("VERDICT: %s\n", result.verdict);
    print_separator();
    printf("\n");
}

int main(int argc, char *argv[]) {
    printf("\n");
    print_separator();
    printf("       CODE PLAGIARISM DETECTOR\n");
    printf("       AST + CFG + DAG Analysis\n");
    print_separator();
    printf("\n");
    
    if (argc < 2) {
        printf("Usage: %s <directory_path>\n", argv[0]);
        printf("   or: %s <file1.c> <file2.c>\n", argv[0]);
        return 1;
    }

    // MODE 1: Direct two-file comparison
    if (argc == 3) {
        printf("Mode: Comparing two files\n\n");
        
        char *code1 = readFile(argv[1]);
        char *code2 = readFile(argv[2]);
        
        if (!code1 || !code2) {
            printf("[ERROR] Could not read one or both files.\n");
            if (code1) free(code1);
            if (code2) free(code2);
            return 1;
        }
        
        PlagiarismResult result = detect_plagiarism(code1, code2);
        print_result(argv[1], argv[2], result);
        
        free(code1);
        free(code2);
        return 0;
    }

    // MODE 2: Directory comparison mode
    printf("Mode: Scanning directory\n\n");

    FileList* list = scanDirectory(argv[1]);  // ✅ uses your directory_handler.c
    if (list == NULL || list->count == 0) {
        printf("No C files found in: %s\n", argv[1]);
        return 1;
    }

    printFileList(list);

    int comparisons = 0;
    int high_plagiarism = 0;
    int medium_similarity = 0;

    for (int i = 0; i < list->count; i++) {
        for (int j = i + 1; j < list->count; j++) {
            printf("\nComparing files %d and %d...\n", i+1, j+1);  // ADD
            char *code1 = readFile(list->paths[i]);
            char *code2 = readFile(list->paths[j]);

            if (!code1 || !code2) {
                printf("[WARN] Could not read file(s): %s or %s\n", list->paths[i], list->paths[j]);
                if (code1) free(code1);
                if (code2) free(code2);
                continue;
            }
        
            printf("\nComparing:\n  %s\n  %s\n", list->paths[i], list->paths[j]);
            printf("------------------------------------------------------------\n");

            
            printf("File 1 size: %zu bytes\n", strlen(code1));
            printf("File 2 size: %zu bytes\n", strlen(code2));

            PlagiarismResult result = detect_plagiarism(code1, code2);

            printf("Detection complete!\n");
            print_result(list->paths[i], list->paths[j], result);
            // ✅ NO MORE PRINTS AFTER THIS

            comparisons++;
            if (result.overall_score >= 0.75)
                high_plagiarism++;
            else if (result.overall_score >= 0.50)
                medium_similarity++;

            free(code1);
            free(code2);
        }
    }

    print_separator();
    printf("SUMMARY\n");
    printf("  Total comparisons:  %d\n", comparisons);
    printf("  High plagiarism:    %d\n", high_plagiarism);
    printf("  Medium similarity:  %d\n", medium_similarity);
    printf("  Low/No similarity:  %d\n", comparisons - high_plagiarism - medium_similarity);
    print_separator();

    freeFileList(list); // ✅ correct cleanup for your version
    return 0;
}
