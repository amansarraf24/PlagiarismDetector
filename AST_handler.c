#include "AST_handler.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define AST_INITIAL_CHILD_CAP 4


static uint64_t mix64(uint64_t x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    x = x ^ (x >> 31);
    return x;
}

ASTNode *ast_new(ASTKind kind, const char *label) {
    ASTNode *n = malloc(sizeof(ASTNode));
    if (!n) {
        fprintf(stderr, "ast_new: out of memory\n");
        exit(1);
    }
    n->kind = kind;
    if (label) {
        n->label = strdup(label);
        if (!n->label) {
            fprintf(stderr, "ast_new: strdup failed\n");
            exit(1);
        }
    } else {
        n->label = NULL;
    }
    n->child_count = 0;
    n->child_capacity = AST_INITIAL_CHILD_CAP;
    n->children = malloc(sizeof(ASTNode *) * n->child_capacity);
    if (!n->children) {
        fprintf(stderr, "ast_new: children alloc failed\n");
        exit(1);
    }
    n->hash = 0;
    return n;
}

void ast_add_child(ASTNode *parent, ASTNode *child) {
    if (!parent || !child) return;
    if (parent->child_count >= parent->child_capacity) {
        parent->child_capacity *= 2;
        ASTNode **tmp = realloc(parent->children,
                                sizeof(ASTNode *) * parent->child_capacity);
        if (!tmp) {
            fprintf(stderr, "ast_add_child: realloc failed\n");
            exit(1);
        }
        parent->children = tmp;
    }
    parent->children[parent->child_count++] = child;
}

void ast_free(ASTNode *node) {
    if (!node) return;
    for (size_t i = 0; i < node->child_count; i++) {
        ast_free(node->children[i]);
    }
    free(node->children);
    if (node->label) free(node->label);
    free(node);
}

void ast_print(ASTNode *node, int indent) {
    if (!node) return;
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
    const char *kname = "UNKNOWN";
    switch (node->kind) {
        case AST_K_NUMBER:        kname = "NUMBER"; break;
        case AST_K_IDENTIFIER:    kname = "IDENT"; break;
        case AST_K_UNARY_OP:      kname = "UNARY"; break;
        case AST_K_BINARY_OP:     kname = "BINARY"; break;
        case AST_K_STATEMENT:     kname = "STMT"; break;
        case AST_K_EXPR_STATEMENT: kname = "EXPR_STMT"; break;
        case AST_K_IF:            kname = "IF"; break;
        case AST_K_WHILE:         kname = "WHILE"; break;
        case AST_K_RETURN:        kname = "RETURN"; break;
        case AST_K_FUNCTION:      kname = "FUNC"; break;
        case AST_K_CALL:          kname = "CALL"; break;
        case AST_K_PROGRAM:       kname = "PROGRAM"; break;
        default: break;
    }
    if (node->label) {
        printf("%s [%s] (children = %zu)\n",
               kname, node->label, node->child_count);
    } else {
        printf("%s (children = %zu)\n",
               kname, node->child_count);
    }
    for (size_t i = 0; i < node->child_count; i++) {
        ast_print(node->children[i], indent + 1);
    }
}

uint64_t ast_compute_hash(ASTNode *node) {
    if (!node) return 0;
    if (node->hash != 0) {
        return node->hash;
    }
    uint64_t h = mix64((uint64_t)node->kind + 0x9e3779b9ULL);
    if (node->label) {
        for (const char *p = node->label; *p; p++) {
            h = mix64(h ^ (uint64_t)(unsigned char)*p);
        }
    }
    for (size_t i = 0; i < node->child_count; i++) {
        uint64_t ch = ast_compute_hash(node->children[i]);
        
        h ^= mix64(ch + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2));
    }
    if (h == 0) h = 1;  
    node->hash = h;
    return h;
}

ASTNode *ast_clone(ASTNode *node) {
    if (!node) return NULL;
    ASTNode *copy = ast_new(node->kind, node->label);
    for (size_t i = 0; i < node->child_count; i++) {
        ASTNode *c = ast_clone(node->children[i]);
        ast_add_child(copy, c);
    }
    return copy;
}

int ast_equal(ASTNode *a, ASTNode *b) {
    if (a == b) return 1;
    if (!a || !b) return 0;
    if (a->kind != b->kind) return 0;
    if ((a->label == NULL) != (b->label == NULL)) return 0;
    if (a->label && b->label && strcmp(a->label, b->label) != 0) return 0;
    if (a->child_count != b->child_count) return 0;
    for (size_t i = 0; i < a->child_count; i++) {
        if (!ast_equal(a->children[i], b->children[i])) return 0;
    }
    return 1;
}