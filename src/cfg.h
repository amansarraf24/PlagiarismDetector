#ifndef CFG_H
#define CFG_H

#include "ast.h"

typedef struct CFGNode {
    int id;
    NodeType type;
    struct CFGNode **successors;
    int successor_count;
    int successor_capacity;
} CFGNode;

typedef struct {
    CFGNode **nodes;
    int node_count;
    int node_capacity;
} ControlFlowGraph;

ControlFlowGraph* build_cfg(ASTNode *ast);
void free_cfg(ControlFlowGraph *cfg);
double compare_cfg(ControlFlowGraph *cfg1, ControlFlowGraph *cfg2);

#endif