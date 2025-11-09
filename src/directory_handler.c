#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <dirent.h>
#endif

#include "directory_handler.h"

int endsWithC(const char* filename) {
    int len = strlen(filename);
    if (len < 3) {
        return 0;
    }
    
    if (filename[len - 2] == '.' && filename[len - 1] == 'c') {
        return 1;
    }
    
    return 0;
}

#ifdef _WIN32

FileList* scanDirectory(const char* directoryPath) {
    FileList* list = (FileList*)malloc(sizeof(FileList));
    if (list == NULL) {
        printf("[ERROR] Memory allocation failed\n");
        return NULL;
    }
    
    list->count = 0;
    
    char searchPath[MAX_PATH_LENGTH];
    snprintf(searchPath, MAX_PATH_LENGTH, "%s\\*.c", directoryPath);
    
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(searchPath, &findData);
    
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("[ERROR] Cannot open directory: %s\n", directoryPath);
        free(list);
        return NULL;
    }
    
    do {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            if (endsWithC(findData.cFileName)) {
                snprintf(list->paths[list->count], MAX_PATH_LENGTH, 
                         "%s\\%s", directoryPath, findData.cFileName);
                list->count++;
                
                if (list->count >= MAX_FILES) {
                    break;
                }
            }
        }
    } while (FindNextFile(hFind, &findData) != 0);
    
    FindClose(hFind);
    
    printf("[OK] Found %d .c files\n", list->count);
    return list;
}

#else

FileList* scanDirectory(const char* directoryPath) {
    FileList* list = (FileList*)malloc(sizeof(FileList));
    if (list == NULL) {
        printf("[ERROR] Memory allocation failed\n");
        return NULL;
    }
    
    list->count = 0;
    
    DIR* dir = opendir(directoryPath);
    if (dir == NULL) {
        printf("[ERROR] Cannot open directory: %s\n", directoryPath);
        free(list);
        return NULL;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL && list->count < MAX_FILES) {
        if (endsWithC(entry->d_name)) {
            snprintf(list->paths[list->count], MAX_PATH_LENGTH, 
                     "%s/%s", directoryPath, entry->d_name);
            list->count++;
        }
    }
    
    closedir(dir);
    
    printf("[OK] Found %d .c files\n", list->count);
    return list;
}

#endif

void printFileList(FileList* list) {
    if (list == NULL) {
        printf("[ERROR] File list is NULL\n");
        return;
    }
    
    printf("\n========================================\n");
    printf("DETECTED C FILES: %d\n", list->count);
    printf("========================================\n");
    
    for (int i = 0; i < list->count; i++) {
        printf("[%d] %s\n", i + 1, list->paths[i]);
    }
    
    printf("========================================\n\n");
}

void freeFileList(FileList* list) {
    if (list != NULL) {
        free(list);
    }
}
// ADD THESE AT THE END OF directory_handler.c

void traverse_directory(const char *path, FileList *files) {
    FileList* result = scanDirectory(path);
    if (result) {
        files->count = result->count;
        for (int i = 0; i < result->count; i++) {
            strcpy(files->paths[i], result->paths[i]);
        }
        freeFileList(result);
    }
}

void cleanup_file_list(FileList *files) {
    // Nothing to free since paths is static array
    files->count = 0;
}