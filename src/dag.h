#ifndef DAG_H
#define DAG_H

#include "ast.h"

typedef struct DAGNode {
    int id;
    NodeType type;
    unsigned long hash;
    struct DAGNode **operands;
    int operand_count;
    int operand_capacity;
} DAGNode;

typedef struct {
    DAGNode **nodes;
    int node_count;
    int node_capacity;
} DirectedAcyclicGraph;

DirectedAcyclicGraph* build_dag(ASTNode *ast);
void free_dag(DirectedAcyclicGraph *dag);
double compare_dag(DirectedAcyclicGraph *dag1, DirectedAcyclicGraph *dag2);

#endif