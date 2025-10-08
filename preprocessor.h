#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

char* removeComments(const char* code);
char* normalizeWhitespace(const char* code);
char* preprocessCode(const char* code);
void printPreprocessedComparison(const char* original, const char* processed);

#endif