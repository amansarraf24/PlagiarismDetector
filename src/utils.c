#include "utils.h"

char* trim_whitespace(char *str) {
    if (!str) return NULL;
    char *end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return str;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

int is_keyword(const char *str) {
    if (!str) return 0;
    
    const char *keywords[] = {
        "if", "else", "while", "for", "do", "switch", "case", "default",
        "break", "continue", "return",
        "int", "float", "char", "double", "void",
        "long", "short", "unsigned", "signed",
        "struct", "union", "enum", "typedef",
        "const", "static", "extern", "auto", "register",
        "sizeof", "goto", NULL
    };
    
    for (int i = 0; keywords[i] != NULL; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    
    return 0;
}

unsigned long string_hash(const char *str) {
    if (!str) return 0;
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

int min_int(int a, int b) { 
    return a < b ? a : b; 
}

int min3_int(int a, int b, int c) { 
    return min_int(min_int(a, b), c); 
}

int max_int(int a, int b) { 
    return a > b ? a : b; 
}

double min_double(double a, double b) { 
    return a < b ? a : b; 
}

double max_double(double a, double b) {
    return a > b ? a : b;
}

