#include "normalizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char original[128];
    char normalized[128];
} VarMapping;

typedef struct {
    VarMapping *mappings;
    int count;
    int capacity;
} VarTable;

static VarTable* create_var_table() {
    VarTable *table = malloc(sizeof(VarTable));
    if (!table) return NULL;
    table->capacity = 100;
    table->count = 0;
    table->mappings = malloc(sizeof(VarMapping) * table->capacity);
    if (!table->mappings) {
        free(table);
        return NULL;
    }
    return table;
}

static void free_var_table(VarTable *table) {
    if (table) {
        if (table->mappings) free(table->mappings);
        free(table);
    }
}

static char* get_normalized_name(VarTable *table, const char *original) {
    if (!table || !original) return NULL;
    
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->mappings[i].original, original) == 0) {
            return table->mappings[i].normalized;
        }
    }
    
    if (table->count >= table->capacity) {
        table->capacity *= 2;
        VarMapping *new_mappings = realloc(table->mappings, 
                                          sizeof(VarMapping) * table->capacity);
        if (!new_mappings) return NULL;
        table->mappings = new_mappings;
    }
    
    VarMapping *map = &table->mappings[table->count];
    strncpy(map->original, original, 127);
    map->original[127] = '\0';
    sprintf(map->normalized, "var_%d", table->count);
    table->count++;
    
    return map->normalized;
}

static ASTNode* switch_to_if_else(ASTNode *switch_node, VarTable *table);

static ASTNode* normalize_recursive(ASTNode *node, VarTable *table) {
    if (!node || !table) return NULL;
    
    // DEBUG print
    // printf("[NORMALIZER] Processing node type=%d, value='%s', children=%d\n", 
    //        node->type, node->value, node->child_count);
    

    // Switch ko if-else mein convert karo
    if (node->type == NODE_SWITCH) {
        return switch_to_if_else(node, table);
    }
    
    // FOR loop ko WHILE mein convert karo (PROPER STRUCTURE CHANGE)
    // ✅ FOR → WHILE conversion fix (improved)
if (node->type == NODE_FOR) {
    // FOR(init; cond; inc; body) → { init; while(cond) { body; inc; } }
    ASTNode *init = NULL, *cond = NULL, *inc = NULL, *body = NULL;

    if (node->child_count > 0) init = node->children[0];
    if (node->child_count > 1) cond = node->children[1];
    if (node->child_count > 2) inc = node->children[2];
    if (node->child_count > 3) body = node->children[3];

    // Step 1: create while(cond)
    ASTNode *while_node = create_node(NODE_WHILE, "loop");
    if (cond) {
        ASTNode *cond_n = normalize_recursive(cond, table);
        if (cond_n) add_child(while_node, cond_n);
    } else {
        add_child(while_node, create_node(NODE_LITERAL, "1"));
    }

    // Step 2: create while body block { body; inc; }
    ASTNode *while_body = create_node(NODE_BLOCK, "block");

    if (body) {
        ASTNode *b = normalize_recursive(body, table);
        if (b) {
            if (b->type == NODE_BLOCK) {
                for (int i = 0; i < b->child_count; i++)
                    add_child(while_body, b->children[i]);
            } else add_child(while_body, b);
        }
    }

    if (inc) {
        ASTNode *i = normalize_recursive(inc, table);
        if (i) add_child(while_body, i);
    }

    add_child(while_node, while_body);

    // Step 3: Create final outer block { init; while(...) {...} }
    ASTNode *final_block = create_node(NODE_BLOCK, "block");
    if (init) {
        ASTNode *init_n = normalize_recursive(init, table);
        if (init_n) add_child(final_block, init_n);
    }
    add_child(final_block, while_node);

    
    return final_block;
}



    
    // Normal node processing
    ASTNode *new_node = create_node(node->type, node->value);
    if (!new_node) return NULL;
    
    // Variable names normalize karo
    if (node->type == NODE_VAR || node->type == NODE_ARRAY_ACCESS) {
        if (strcmp(node->value, "long") == 0 || 
            strcmp(node->value, "short") == 0 ||
            strcmp(node->value, "unsigned") == 0 ||
            strcmp(node->value, "signed") == 0) {
            strcpy(new_node->value, "int");
        }
        else if (strcmp(node->value, "double") == 0) {
            strcpy(new_node->value, "float");
        }
        else {
            char *normalized = get_normalized_name(table, node->value);
            if (normalized) {
                strncpy(new_node->value, normalized, 127);
                new_node->value[127] = '\0';
            }
        }
    }
    
    // WHILE loop normalize karo
    if (node->type == NODE_WHILE) {
        strcpy(new_node->value, "loop");
    }
    
    // ✅ Unify FOR, WHILE, DO loops as "loop" for similarity comparison
if (node->type == NODE_FOR || node->type == NODE_WHILE) {
    strcpy(new_node->value, "loop");
}


    // Recursively process ALL children
    for (int i = 0; i < node->child_count; i++) {
        ASTNode *child = normalize_recursive(node->children[i], table);
        if (child) add_child(new_node, child);
    }
    
    
    return new_node;
}


static ASTNode* switch_to_if_else(ASTNode *switch_node, VarTable *table) {
    if (!switch_node || switch_node->child_count < 2) return switch_node;
    
    ASTNode *switch_expr = switch_node->children[0];
    ASTNode *if_chain = NULL;
    ASTNode *current_if = NULL;
    
    for (int i = 1; i < switch_node->child_count; i++) {
        ASTNode *case_node = switch_node->children[i];
        
        if (case_node->type != NODE_CASE || case_node->child_count < 2) continue;
        
        ASTNode *case_value = case_node->children[0];
        ASTNode *case_body = case_node->children[1];
        
        ASTNode *if_node = create_node(NODE_IF, "if");
        if (!if_node) continue;
        
        ASTNode *condition = create_node(NODE_BINOP, "==");
        if (condition) {
            ASTNode *expr_clone = clone_ast(switch_expr);
            ASTNode *val_clone = clone_ast(case_value);
            if (expr_clone) add_child(condition, expr_clone);
            if (val_clone) add_child(condition, val_clone);
            add_child(if_node, condition);
        }
        
        ASTNode *body_clone = clone_ast(case_body);
        if (body_clone) add_child(if_node, body_clone);
        
        if (!if_chain) {
            if_chain = if_node;
            current_if = if_node;
        } else {
            if (current_if) add_child(current_if, if_node);
            current_if = if_node;
        }
    }
    
    return if_chain ? normalize_recursive(if_chain, table) : switch_node;
}

ASTNode* normalize_ast(ASTNode *ast) {
    if (!ast) return NULL;
    printf("[DEBUG] Running FOR→WHILE normalization...\n");

    // ADD THESE DEBUG LINES:
    printf("[NORMALIZER DEBUG] Starting normalization\n");
    printf("[NORMALIZER DEBUG] Root type: %d, children: %d\n", 
           ast->type, ast->child_count);
    
    for (int i = 0; i < ast->child_count; i++) {
        if (ast->children[i]) {
            printf("[NORMALIZER DEBUG] Child[%d]: type=%d, value='%s', children=%d\n", 
                   i, ast->children[i]->type, ast->children[i]->value, 
                   ast->children[i]->child_count);
        } else {
            printf("[NORMALIZER DEBUG] Child[%d]: NULL\n", i);
        }
    }
    
    VarTable *table = create_var_table();
    if (!table) return NULL;
    
    ASTNode *normalized = normalize_recursive(ast, table);
    printf("[DEBUG] AST normalized successfully: %d nodes\n", count_nodes(normalized));

    free_var_table(table);
    
    return normalized;
}