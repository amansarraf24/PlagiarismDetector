#include <stdio.h>
#include <stdlib.h>
#include "file_handler.h"

int fileExists(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return 0;
    }
    fclose(file);
    return 1;
}

long getFileSize(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        return -1;
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);
    
    return size;
}

char* readFile(const char* filename) {
    if (!fileExists(filename)) {
        printf("[ERROR] File not found: %s\n", filename);
        return NULL;
    }
    
    long fileSize = getFileSize(filename);
    if (fileSize <= 0) {
        printf("[ERROR] Invalid file size\n");
        return NULL;
    }
    
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("[ERROR] Cannot open file\n");
        return NULL;
    }
    
    char* content = (char*)malloc(fileSize + 1);
    if (content == NULL) {
        printf("[ERROR] Memory allocation failed\n");
        fclose(file);
        return NULL;
    }
    
    size_t bytesRead = fread(content, 1, fileSize, file);
    content[bytesRead] = '\0';
    
    fclose(file);
    return content;
}
// ADD THIS FUNCTION AT THE END OF file_handler.c

char* read_file(const char *filename) {
    return readFile(filename);  // Just calls your existing function
}