#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ASTNode* create_node(NodeType type, const char *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = type;
    if (value) {
        strncpy(node->value, value, 127);
        node->value[127] = '\0';
    } else {
        node->value[0] = '\0';
    }
    
    node->child_capacity = 8;
    node->children = malloc(sizeof(ASTNode*) * node->child_capacity);
    if (!node->children) {
        free(node);
        return NULL;
    }
    node->child_count = 0;
    
    return node;
}

void add_child(ASTNode *parent, ASTNode *child) {
    if (!parent || !child) return;
    
    if (parent->child_count >= parent->child_capacity) {
        parent->child_capacity *= 2;
        ASTNode **new_children = realloc(parent->children, 
                                        sizeof(ASTNode*) * parent->child_capacity);
        if (!new_children) {
            fprintf(stderr, "Memory reallocation failed\n");
            return;
        }
        parent->children = new_children;
    }
    
    parent->children[parent->child_count++] = child;
}

void free_ast(ASTNode *node) {
    if (!node) return;
    
    for (int i = 0; i < node->child_count; i++) {
        free_ast(node->children[i]);
    }
    
    if (node->children) free(node->children);
    free(node);
}

int count_nodes(ASTNode *node) {
    if (!node) return 0;
    
    int count = 1;
    for (int i = 0; i < node->child_count; i++) {
        count += count_nodes(node->children[i]);
    }
    return count;
}

ASTNode* clone_ast(ASTNode *node) {
    if (!node) return NULL;
    
    ASTNode *clone = create_node(node->type, node->value);
    if (!clone) return NULL;
    
    for (int i = 0; i < node->child_count; i++) {
        ASTNode *child_clone = clone_ast(node->children[i]);
        if (child_clone) {
            add_child(clone, child_clone);
        }
    }
    
    return clone;
}