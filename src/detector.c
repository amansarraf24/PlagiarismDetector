#include "detector.h"
#include "lexer.h"
#include "parser.h"
#include "normalizer.h"
#include "cfg.h"
#include "dag.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_ast_debug(ASTNode *node, int depth) {
    if (!node) {
        for (int i = 0; i < depth; i++) printf("  ");
        printf("NULL\n");
        return;
    }
    
    for (int i = 0; i < depth; i++) printf("  ");
    
    const char *type_names[] = {
        "PROGRAM", "IF", "WHILE", "FOR", "DO_WHILE", "SWITCH", "CASE",
        "ASSIGN", "BINOP", "VAR", "LITERAL", "ARRAY_ACCESS", "RETURN",
        "BREAK", "CONTINUE", "BLOCK"
    };
    
    printf("[%s] value='%s' children=%d\n", 
           type_names[node->type], node->value, node->child_count);
    
    for (int i = 0; i < node->child_count; i++) {
        print_ast_debug(node->children[i], depth + 1);
    }
}

static int tree_edit_distance(ASTNode *t1, ASTNode *t2) {
    if (!t1 && !t2) return 0;
    if (!t1) return count_nodes(t2);
    if (!t2) return count_nodes(t1);
    
    int cost = (t1->type == t2->type) ? 0 : 1;
    
    if (t1->child_count == 0 && t2->child_count == 0) {
        return cost;
    }
    
    int total_cost = cost;
    int max_children = max_int(t1->child_count, t2->child_count);
    
    for (int i = 0; i < max_children; i++) {
        ASTNode *child1 = (i < t1->child_count) ? t1->children[i] : NULL;
        ASTNode *child2 = (i < t2->child_count) ? t2->children[i] : NULL;
        total_cost += tree_edit_distance(child1, child2);
    }
    
    return total_cost;
}

static double calculate_ast_similarity(ASTNode *t1, ASTNode *t2) {
    if (!t1 || !t2) return 0.0;
    
    int distance = tree_edit_distance(t1, t2);
    int max_size = max_int(count_nodes(t1), count_nodes(t2));
    
    if (max_size == 0) return 1.0;
    
    return 1.0 - ((double)distance / (max_size * 1.5));
}

static void determine_verdict(PlagiarismResult *result) {
    if (!result) return;
    
    if (result->overall_score >= 0.85) {
        strcpy(result->verdict, "HIGH PLAGIARISM - Almost identical code");
    } else if (result->overall_score >= 0.75) {
        strcpy(result->verdict, "HIGH Similarity - Likely plagiarized");
    } else if (result->overall_score >= 0.60) {
        strcpy(result->verdict, "MEDIUM Similarity - Same logic, different style");
    } else if (result->overall_score >= 0.40) {
        strcpy(result->verdict, "LOW-MEDIUM Similarity - Some common patterns");
    } else if (result->overall_score >= 0.25) {
        strcpy(result->verdict, "LOW Similarity - Different approaches");
    } else {
        strcpy(result->verdict, "MINIMAL Similarity - Likely different code");
    }
}

PlagiarismResult detect_plagiarism(const char *code1, const char *code2) {
    PlagiarismResult result = {0};
    strcpy(result.verdict, "Unable to analyze");
    
    if (!code1 || !code2) {
        printf("[DEBUG] NULL input\n");  // ADD
        strcpy(result.verdict, "NULL input");
        return result;
    }
    
    if (strlen(code1) == 0 || strlen(code2) == 0) {
        strcpy(result.verdict, "Empty code");
        return result;
    }
    
    if (strcmp(code1, code2) == 0) {
        result.overall_score = 1.0;
        result.ast_similarity = 1.0;
        result.cfg_similarity = 1.0;
        result.dag_similarity = 1.0;
        strcpy(result.verdict, "EXACT COPY - 100% identical");
        return result;
    }
    
    printf("[DEBUG] Tokenizing...\n");  // ADD
    TokenList *tokens1 = tokenize(code1);
    TokenList *tokens2 = tokenize(code2);
    
    if (!tokens1 || !tokens2 || tokens1->count < 5 || tokens2->count < 5) {
        if (tokens1) free_tokens(tokens1);
        if (tokens2) free_tokens(tokens2);
        strcpy(result.verdict, "Code too small (less than 5 tokens)");
        return result;
    }
    
    printf("[DEBUG] Parsing...\n");
ASTNode *ast1 = parse(tokens1);
ASTNode *ast2 = parse(tokens2);

// ADD THESE DEBUG LINES:
printf("[DEBUG PARSER] AST1 root type: %d, children: %d\n", 
       ast1 ? ast1->type : -1, ast1 ? ast1->child_count : 0);
if (ast1 && ast1->child_count > 0) {
    for (int i = 0; i < ast1->child_count; i++) {
        printf("[DEBUG PARSER] AST1 child[%d] type: %d\n", i, 
               ast1->children[i] ? ast1->children[i]->type : -1);
    }
}
    
    if (!ast1 || !ast2) {
        if (ast1) free_ast(ast1);
        if (ast2) free_ast(ast2);
        free_tokens(tokens1);
        free_tokens(tokens2);
        strcpy(result.verdict, "Failed to parse - syntax errors");
        return result;
    }
    
    result.total_nodes_1 = count_nodes(ast1);
    result.total_nodes_2 = count_nodes(ast2);

if (result.total_nodes_1 < 3 || result.total_nodes_2 < 3) {
    free_ast(ast1);
    free_ast(ast2);
    free_tokens(tokens1);
    free_tokens(tokens2);
    strcpy(result.verdict, "Code too simple (less than 3 nodes)");
    return result;
}

printf("[DEBUG] Normalizing...\n");
printf("[DEBUG] AST1 before normalization: %d nodes\n", result.total_nodes_1);
printf("[DEBUG] AST2 before normalization: %d nodes\n", result.total_nodes_2);

ASTNode *norm1 = normalize_ast(ast1);
ASTNode *norm2 = normalize_ast(ast2);

if (!norm1 || !norm2) {
    if (norm1) free_ast(norm1);
    if (norm2) free_ast(norm2);
    free_ast(ast1);
    free_ast(ast2);
    free_tokens(tokens1);
    free_tokens(tokens2);
    strcpy(result.verdict, "Normalization failed");
    return result;
}

// UPDATE node counts AFTER normalization
int norm_nodes_1 = count_nodes(norm1);
int norm_nodes_2 = count_nodes(norm2);
printf("[DEBUG] AST1 after normalization: %d nodes\n", norm_nodes_1);
printf("[DEBUG] AST2 after normalization: %d nodes\n", norm_nodes_2);

// ✅ ADD THESE LINES:
printf("\n[AST DEBUG] Printing AST1 structure:\n");
print_ast_debug(norm1, 0);
printf("\n[AST DEBUG] Printing AST2 structure:\n");
print_ast_debug(norm2, 0);
printf("\n");
    
    result.ast_similarity = calculate_ast_similarity(norm1, norm2);
    
    printf("[DEBUG] Building CFG...\n");  // ADD
    ControlFlowGraph *cfg1 = build_cfg(norm1);
    ControlFlowGraph *cfg2 = build_cfg(norm2);
    
    if (cfg1 && cfg2 && cfg1->node_count > 2 && cfg2->node_count > 2) {
        result.cfg_similarity = compare_cfg(cfg1, cfg2);
    } else {
        result.cfg_similarity = result.ast_similarity * 0.9;
    }
    
    printf("[DEBUG] Building DAG...\n");  // ADD
    DirectedAcyclicGraph *dag1 = build_dag(norm1);
    DirectedAcyclicGraph *dag2 = build_dag(norm2);
    
    if (dag1 && dag2 && dag1->node_count > 0 && dag2->node_count > 0) {
        result.dag_similarity = compare_dag(dag1, dag2);
    } else {
        result.dag_similarity = result.ast_similarity * 0.85;
    }
    
    int avg_nodes = (result.total_nodes_1 + result.total_nodes_2) / 2;
    double w_ast, w_cfg, w_dag;
    
    if (avg_nodes < 10) {
        w_ast = 0.50;
        w_cfg = 0.30;
        w_dag = 0.20;
    } else if (avg_nodes < 30) {
        w_ast = 0.35;
        w_cfg = 0.35;
        w_dag = 0.30;
    } else {
        w_ast = 0.25;
        w_cfg = 0.40;
        w_dag = 0.35;
    }
    
    result.overall_score = (w_ast * result.ast_similarity) +
                          (w_cfg * result.cfg_similarity) +
                          (w_dag * result.dag_similarity);
    
    double min_score = min_double(result.ast_similarity,
                                  min_double(result.cfg_similarity, result.dag_similarity));
    double max_score = max_double(result.ast_similarity,
                                  max_double(result.cfg_similarity, result.dag_similarity));
    
    double variance = max_score - min_score;
    
    if (variance < 0.15 && result.overall_score > 0.6) {
        result.overall_score = min_double(1.0, result.overall_score * 1.08);
    } else if (variance > 0.35) {
        result.overall_score *= 0.92;
    }
    
    double size_ratio = (double)min_int(result.total_nodes_1, result.total_nodes_2) /
                        (double)max_int(result.total_nodes_1, result.total_nodes_2);
    
    if (size_ratio < 0.4) {
        result.overall_score *= 0.75;
    } else if (size_ratio < 0.6) {
        result.overall_score *= 0.90;
    }
    
    if (result.overall_score < 0.0) result.overall_score = 0.0;
    if (result.overall_score > 1.0) result.overall_score = 1.0;
    
    determine_verdict(&result);
    
    free_tokens(tokens1);
    free_tokens(tokens2);
    free_ast(ast1);
    free_ast(ast2);
    free_ast(norm1);
    free_ast(norm2);
    if (cfg1) free_cfg(cfg1);
    if (cfg2) free_cfg(cfg2);
    if (dag1) free_dag(dag1);
    if (dag2) free_dag(dag2);
    
    return result;
}