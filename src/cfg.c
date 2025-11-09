#include "cfg.h"
#include "utils.h"
#include <stdlib.h>

static int node_id = 0;

static CFGNode* create_cfg_node(NodeType type) {
    CFGNode *node = malloc(sizeof(CFGNode));
    if (!node) return NULL;
    
    node->id = node_id++;
    node->type = type;
    node->successor_capacity = 8;
    node->successors = malloc(sizeof(CFGNode*) * node->successor_capacity);
    if (!node->successors) {
        free(node);
        return NULL;
    }
    node->successor_count = 0;
    return node;
}

static void add_successor(CFGNode *from, CFGNode *to) {
    if (!from || !to) return;
    
    for (int i = 0; i < from->successor_count; i++) {
        if (from->successors[i] == to) return;
    }
    
    if (from->successor_count >= from->successor_capacity) {
        from->successor_capacity *= 2;
        CFGNode **new_successors = realloc(from->successors,
                                          sizeof(CFGNode*) * from->successor_capacity);
        if (!new_successors) return;
        from->successors = new_successors;
    }
    
    from->successors[from->successor_count++] = to;
}

static void add_node_to_cfg(ControlFlowGraph *cfg, CFGNode *node) {
    if (!cfg || !node) return;
    
    if (cfg->node_count >= cfg->node_capacity) {
        cfg->node_capacity *= 2;
        CFGNode **new_nodes = realloc(cfg->nodes, 
                                     sizeof(CFGNode*) * cfg->node_capacity);
        if (!new_nodes) return;
        cfg->nodes = new_nodes;
    }
    
    cfg->nodes[cfg->node_count++] = node;
}

static CFGNode* process_node(ASTNode *node, CFGNode *current, ControlFlowGraph *cfg);

static CFGNode* process_node(ASTNode *node, CFGNode *current, ControlFlowGraph *cfg) {
    if (!node || !current || !cfg) return current;
    
    switch (node->type) {
        case NODE_ASSIGN: {
            CFGNode *assign = create_cfg_node(NODE_ASSIGN);
            if (!assign) return current;
            add_successor(current, assign);
            add_node_to_cfg(cfg, assign);
            return assign;
        }
        
        case NODE_IF: {
            CFGNode *cond = create_cfg_node(NODE_IF);
            if (!cond) return current;
            add_successor(current, cond);
            add_node_to_cfg(cfg, cond);
            
            CFGNode *merge = create_cfg_node(NODE_BLOCK);
            if (!merge) return current;
            add_node_to_cfg(cfg, merge);
            
            CFGNode *then_end = cond;
            if (node->child_count > 1) {
                then_end = process_node(node->children[1], cond, cfg);
            }
            add_successor(then_end, merge);
            
            if (node->child_count > 2) {
                CFGNode *else_end = process_node(node->children[2], cond, cfg);
                add_successor(else_end, merge);
            } else {
                add_successor(cond, merge);
            }
            
            return merge;
        }
        
        case NODE_WHILE: {
            CFGNode *loop = create_cfg_node(NODE_WHILE);
            if (!loop) return current;
            add_successor(current, loop);
            add_node_to_cfg(cfg, loop);
            
            // WHILE structure: child[0]=condition, child[1]=body
            CFGNode *body_end = loop;
            if (node->child_count > 1 && node->children[1]) {
                body_end = process_node(node->children[1], loop, cfg);
            }
            
            // Back edge: body loops to condition
            add_successor(body_end, loop);
            
            // Exit edge: loop exits after condition fails
            CFGNode *exit = create_cfg_node(NODE_BLOCK);
            if (!exit) return current;
            add_successor(loop, exit);
            add_node_to_cfg(cfg, exit);
            
            return exit;
        }
        
        case NODE_BLOCK: {
            CFGNode *prev = current;
            for (int i = 0; i < node->child_count; i++) {
                prev = process_node(node->children[i], prev, cfg);
            }
            return prev;
        }
        
        case NODE_RETURN: {
            CFGNode *ret = create_cfg_node(NODE_RETURN);
            if (!ret) return current;
            add_successor(current, ret);
            add_node_to_cfg(cfg, ret);
            return ret;
        }
        
        default:
            return current;
    }
}

ControlFlowGraph* build_cfg(ASTNode *ast) {
    if (!ast) return NULL;
    
    node_id = 0;
    
    ControlFlowGraph *cfg = malloc(sizeof(ControlFlowGraph));
    if (!cfg) return NULL;
    
    cfg->node_capacity = 100;
    cfg->nodes = malloc(sizeof(CFGNode*) * cfg->node_capacity);
    if (!cfg->nodes) {
        free(cfg);
        return NULL;
    }
    cfg->node_count = 0;
    
    CFGNode *entry = create_cfg_node(NODE_PROGRAM);
    if (!entry) {
        free(cfg->nodes);
        free(cfg);
        return NULL;
    }
    add_node_to_cfg(cfg, entry);
    
    CFGNode *current = entry;
    for (int i = 0; i < ast->child_count; i++) {
        current = process_node(ast->children[i], current, cfg);
    }
    
    CFGNode *exit = create_cfg_node(NODE_PROGRAM);
    if (exit) {
        add_node_to_cfg(cfg, exit);
        add_successor(current, exit);
    }
    
    return cfg;
}

void free_cfg(ControlFlowGraph *cfg) {
    if (!cfg) return;
    
    for (int i = 0; i < cfg->node_count; i++) {
        if (cfg->nodes[i]) {
            if (cfg->nodes[i]->successors) free(cfg->nodes[i]->successors);
            free(cfg->nodes[i]);
        }
    }
    
    if (cfg->nodes) free(cfg->nodes);
    free(cfg);
}

double compare_cfg(ControlFlowGraph *cfg1, ControlFlowGraph *cfg2) {
    if (!cfg1 || !cfg2 || cfg1->node_count == 0 || cfg2->node_count == 0) {
        return 0.0;
    }
    
    int matches = 0;
    int min_count = min_int(cfg1->node_count, cfg2->node_count);
    int max_count = max_int(cfg1->node_count, cfg2->node_count);
    
    for (int i = 0; i < min_count; i++) {
        if (cfg1->nodes[i]->type == cfg2->nodes[i]->type) {
            matches++;
        }
    }
    
    double type_sim = (double)matches / max_count;
    
    int edge_matches = 0;
    int total_edges = 0;
    
    for (int i = 0; i < min_count; i++) {
        int s1 = cfg1->nodes[i]->successor_count;
        int s2 = cfg2->nodes[i]->successor_count;
        total_edges += max_int(s1, s2);
        edge_matches += min_int(s1, s2);
    }
    
    double edge_sim = (total_edges > 0) ? (double)edge_matches / total_edges : 0.0;
    
    return 0.6 * type_sim + 0.4 * edge_sim;
}