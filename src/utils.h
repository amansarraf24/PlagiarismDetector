#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char* trim_whitespace(char *str);
int is_keyword(const char *word);
unsigned long string_hash(const char *str);
int min_int(int a, int b);
int min3_int(int a, int b, int c);
int max_int(int a, int b);
double min_double(double a, double b);
double max_double(double a, double b);
int is_keyword(const char *str);

#endif