#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

char* read_file(const char *filename);
int fileExists(const char* filename);
long getFileSize(const char* filename);
char* readFile(const char* filename);

#endif