#ifndef DIRECTORY_HANDLER_H
#define DIRECTORY_HANDLER_H

#define MAX_FILES 100
#define MAX_PATH_LENGTH 512

typedef struct {
    char paths[MAX_FILES][MAX_PATH_LENGTH];
    int count;
} FileList;

void traverse_directory(const char *path, FileList *files);
void cleanup_file_list(FileList *files);

FileList* scanDirectory(const char* directoryPath);
void freeFileList(FileList* list);
void printFileList(FileList* list);
int endsWithC(const char* filename);

#endif