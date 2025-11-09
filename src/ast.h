#ifndef AST_H
#define AST_H

typedef enum {
    NODE_PROGRAM,
    NODE_IF,
    NODE_WHILE,
    NODE_FOR,
    NODE_DO_WHILE,
    NODE_SWITCH,
    NODE_CASE,
    NODE_ASSIGN,
    NODE_BINOP,
    NODE_VAR,
    NODE_LITERAL,
    NODE_ARRAY_ACCESS,
    NODE_RETURN,
    NODE_BREAK,
    NODE_CONTINUE,
    NODE_BLOCK
} NodeType;

typedef struct ASTNode {
    NodeType type;
    char value[128];
    struct ASTNode **children;
    int child_count;
    int child_capacity;
} ASTNode;

ASTNode* create_node(NodeType type, const char *value);
void add_child(ASTNode *parent, ASTNode *child);
void free_ast(ASTNode *node);
int count_nodes(ASTNode *node);
ASTNode* clone_ast(ASTNode *node);

#endif