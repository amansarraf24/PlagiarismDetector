#include "dag.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

static int dag_id = 0;

static DAGNode* create_dag_node(NodeType type, const char *value) {
    DAGNode *node = malloc(sizeof(DAGNode));
    if (!node) return NULL;
    
    node->id = dag_id++;
    node->type = type;
    node->hash = string_hash(value);
    node->operand_capacity = 8;
    node->operands = malloc(sizeof(DAGNode*) * node->operand_capacity);
    if (!node->operands) {
        free(node);
        return NULL;
    }
    node->operand_count = 0;
    return node;
}

static void add_operand(DAGNode *parent, DAGNode *child) {
    if (!parent || !child) return;
    
    if (parent->operand_count >= parent->operand_capacity) {
        parent->operand_capacity *= 2;
        DAGNode **new_operands = realloc(parent->operands,
                                        sizeof(DAGNode*) * parent->operand_capacity);
        if (!new_operands) return;
        parent->operands = new_operands;
    }
    
    parent->operands[parent->operand_count++] = child;
    // Order-independent hash for commutative operations
if (parent->type == NODE_BINOP) {
    parent->hash ^= child->hash;  // XOR for order-independence
} else {
    parent->hash = parent->hash * 31 + child->hash;
}
}

static void add_node_to_dag(DirectedAcyclicGraph *dag, DAGNode *node) {
    if (!dag || !node) return;
    
    if (dag->node_count >= dag->node_capacity) {
        dag->node_capacity *= 2;
        DAGNode **new_nodes = realloc(dag->nodes,
                                     sizeof(DAGNode*) * dag->node_capacity);
        if (!new_nodes) return;
        dag->nodes = new_nodes;
    }
    
    dag->nodes[dag->node_count++] = node;
}

static DAGNode* find_node(DirectedAcyclicGraph *dag, unsigned long hash) {
    if (!dag) return NULL;
    
    for (int i = 0; i < dag->node_count; i++) {
        if (dag->nodes[i]->hash == hash) {
            return dag->nodes[i];
        }
    }
    return NULL;
}

static DAGNode* ast_to_dag(ASTNode *node, DirectedAcyclicGraph *dag) {
    if (!node || !dag) return NULL;
    
    DAGNode *dag_node = create_dag_node(node->type, node->value);
    if (!dag_node) return NULL;
    
    for (int i = 0; i < node->child_count; i++) {
        DAGNode *child = ast_to_dag(node->children[i], dag);
        if (child) add_operand(dag_node, child);
    }
    
    DAGNode *existing = find_node(dag, dag_node->hash);
    if (existing) {
        if (dag_node->operands) free(dag_node->operands);
        free(dag_node);
        return existing;
    }
    
    add_node_to_dag(dag, dag_node);
    return dag_node;
}

DirectedAcyclicGraph* build_dag(ASTNode *ast) {
    if (!ast) return NULL;
    
    dag_id = 0;
    
    DirectedAcyclicGraph *dag = malloc(sizeof(DirectedAcyclicGraph));
    if (!dag) return NULL;
    
    dag->node_capacity = 100;
    dag->nodes = malloc(sizeof(DAGNode*) * dag->node_capacity);
    if (!dag->nodes) {
        free(dag);
        return NULL;
    }
    dag->node_count = 0;
    
    for (int i = 0; i < ast->child_count; i++) {
        ast_to_dag(ast->children[i], dag);
    }
    
    return dag;
}

void free_dag(DirectedAcyclicGraph *dag) {
    if (!dag) return;
    
    for (int i = 0; i < dag->node_count; i++) {
        if (dag->nodes[i]) {
            if (dag->nodes[i]->operands) free(dag->nodes[i]->operands);
            free(dag->nodes[i]);
        }
    }
    
    if (dag->nodes) free(dag->nodes);
    free(dag);
}

double compare_dag(DirectedAcyclicGraph *dag1, DirectedAcyclicGraph *dag2) {
    if (!dag1 || !dag2 || dag1->node_count == 0 || dag2->node_count == 0) {
        return 0.0;
    }
    
    int hash_matches = 0;
    int total = dag1->node_count + dag2->node_count;
    
    for (int i = 0; i < dag1->node_count; i++) {
        for (int j = 0; j < dag2->node_count; j++) {
            if (dag1->nodes[i]->hash == dag2->nodes[j]->hash) {
                hash_matches += 2;
                break;
            }
        }
    }
    
    double hash_sim = (total > 0) ? (double)hash_matches / total : 0.0;
    
    int type_matches = 0;
    int min_count = min_int(dag1->node_count, dag2->node_count);
    
    for (int i = 0; i < min_count; i++) {
        if (dag1->nodes[i]->type == dag2->nodes[i]->type) {
            type_matches++;
        }
    }
    
    double type_sim = (double)type_matches / max_int(dag1->node_count, dag2->node_count);
    
    return 0.7 * hash_sim + 0.3 * type_sim;
}