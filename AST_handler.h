#ifndef AST_H
#define AST_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    AST_K_NUMBER,
    AST_K_IDENTIFIER,
    AST_K_UNARY_OP,
    AST_K_BINARY_OP,
    AST_K_STATEMENT,
    AST_K_EXPR_STATEMENT,
    AST_K_IF,
    AST_K_WHILE,
    AST_K_RETURN,
    AST_K_FUNCTION,
    AST_K_CALL,
    AST_K_PROGRAM,
    
} ASTKind;

typedef struct ASTNode {
    ASTKind kind;
    char *label;                
    struct ASTNode **children; 
    size_t child_count;
    size_t child_capacity;
    uint64_t hash;              
} ASTNode;

ASTNode *ast_new(ASTKind kind, const char *label);

void ast_add_child(ASTNode *parent, ASTNode *child);

void ast_free(ASTNode *node);

void ast_print(ASTNode *node, int indent);

uint64_t ast_compute_hash(ASTNode *node);

ASTNode *ast_clone(ASTNode *node);


int ast_equal(ASTNode *a, ASTNode *b);

#endif 