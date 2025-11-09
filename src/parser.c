#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    Token *tokens;
    int pos;
    int count;
} Parser;

static ASTNode* parse_statement(Parser *p);
static ASTNode* parse_expression(Parser *p);
static ASTNode* parse_comparison(Parser *p);
static ASTNode* parse_additive(Parser *p);
static ASTNode* parse_multiplicative(Parser *p);
static ASTNode* parse_primary(Parser *p);
static ASTNode* parse_block(Parser *p);

static Token* current_token(Parser *p) {
    if (p->pos < p->count) return &p->tokens[p->pos];
    return &p->tokens[p->count - 1];
}

static void advance(Parser *p) {
    if (p->pos < p->count - 1) p->pos++;
}

static int match(Parser *p, TokenType type) {
    return current_token(p)->type == type;
}

static int match_keyword(Parser *p, const char *keyword) {
    Token *tok = current_token(p);
    return tok->type == TOK_KEYWORD && strcmp(tok->value, keyword) == 0;
}

static ASTNode* parse_primary(Parser *p) {
    Token *tok = current_token(p);
    
    if (match(p, TOK_LPAREN)) {
        advance(p);
        ASTNode *expr = parse_comparison(p);
        if (match(p, TOK_RPAREN)) advance(p);
        return expr;
    }
    
    if (match(p, TOK_NUMBER)) {
        ASTNode *node = create_node(NODE_LITERAL, tok->value);
        advance(p);
        return node;
    }
    
    if (match(p, TOK_IDENTIFIER)) {
        char var_name[128];
        strncpy(var_name, tok->value, 127);
        var_name[127] = '\0';
        advance(p);
        
        if (match(p, TOK_LBRACKET)) {
            advance(p);
            ASTNode *arr_node = create_node(NODE_ARRAY_ACCESS, var_name);
            ASTNode *index = parse_comparison(p);
            if (index) add_child(arr_node, index);
            if (match(p, TOK_RBRACKET)) advance(p);
            return arr_node;
        }
        
        return create_node(NODE_VAR, var_name);
    }
    
    return NULL;
}

static ASTNode* parse_multiplicative(Parser *p) {
    ASTNode *left = parse_primary(p);
    if (!left) return NULL;
    
    while (match(p, TOK_OPERATOR)) {
        Token *op = current_token(p);
        if (strcmp(op->value, "*") != 0 && 
            strcmp(op->value, "/") != 0 && 
            strcmp(op->value, "%") != 0) {
            break;
        }
        
        advance(p);
        ASTNode *right = parse_primary(p);
        if (!right) break;
        
        ASTNode *binop = create_node(NODE_BINOP, op->value);
        if (binop) {
            add_child(binop, left);
            add_child(binop, right);
            left = binop;
        }
    }
    
    return left;
}

static ASTNode* parse_additive(Parser *p) {
    ASTNode *left = parse_multiplicative(p);
    if (!left) return NULL;
    
    while (match(p, TOK_OPERATOR)) {
        Token *op = current_token(p);
        if (strcmp(op->value, "+") != 0 && strcmp(op->value, "-") != 0) {
            break;
        }
        
        advance(p);
        ASTNode *right = parse_multiplicative(p);
        if (!right) break;
        
        ASTNode *binop = create_node(NODE_BINOP, op->value);
        if (binop) {
            add_child(binop, left);
            add_child(binop, right);
            left = binop;
        }
    }
    
    return left;
}

static ASTNode* parse_comparison(Parser *p) {
    ASTNode *left = parse_additive(p);
    if (!left) return NULL;
    
    while (match(p, TOK_OPERATOR)) {
        Token *op = current_token(p);
        if (strcmp(op->value, "<") != 0 && strcmp(op->value, ">") != 0 &&
            strcmp(op->value, "<=") != 0 && strcmp(op->value, ">=") != 0 &&
            strcmp(op->value, "==") != 0 && strcmp(op->value, "!=") != 0 &&
            strcmp(op->value, "&&") != 0 && strcmp(op->value, "||") != 0) {
            break;
        }
        
        advance(p);
        ASTNode *right = parse_additive(p);
        if (!right) break;
        
        ASTNode *binop = create_node(NODE_BINOP, op->value);
        if (binop) {
            add_child(binop, left);
            add_child(binop, right);
            left = binop;
        }
    }
    
    return left;
}

static ASTNode* parse_expression(Parser *p) {
    return parse_comparison(p);
}

static ASTNode* parse_block(Parser *p) {
    if (!match(p, TOK_LBRACE)) {
        return parse_statement(p);
    }
    
    advance(p);
    ASTNode *block = create_node(NODE_BLOCK, "block");
    if (!block) return NULL;
    
    while (!match(p, TOK_RBRACE) && current_token(p)->type != TOK_EOF) {
    ASTNode *stmt = parse_statement(p);
    if (stmt) {
        add_child(block, stmt);
    } else {
        // If statement parsing fails, skip this token to avoid infinite loop
        advance(p);
    }
}
    
    if (match(p, TOK_RBRACE)) advance(p);
    return block;
}

static ASTNode* parse_statement(Parser *p) {
    Token *tok = current_token(p);
    
    if (match_keyword(p, "int") || match_keyword(p, "float") || 
    match_keyword(p, "char") || match_keyword(p, "double") ||
    match_keyword(p, "long") || match_keyword(p, "short") ||
    match_keyword(p, "unsigned") || match_keyword(p, "signed") ||
    match_keyword(p, "void")) {  // ← void bhi add karo
    
    advance(p);
    
    while (match_keyword(p, "int") || match_keyword(p, "long") ||
           match_keyword(p, "short") || match_keyword(p, "unsigned") ||
           match_keyword(p, "signed")) {
        advance(p);
    }
    
    if (!match(p, TOK_IDENTIFIER)) return NULL;
    Token *var_name = current_token(p);
    advance(p);
    
    // ✅ CHECK IF THIS IS A FUNCTION
    if (match(p, TOK_LPAREN)) {
        // This is a function declaration/definition
        advance(p);  // skip '('
        
        // Skip parameters (simplified - just skip until ')')
        while (!match(p, TOK_RPAREN) && current_token(p)->type != TOK_EOF) {
            advance(p);
        }
        
        if (match(p, TOK_RPAREN)) advance(p);  // skip ')'
        
        // Check if function has body
        if (match(p, TOK_LBRACE)) {
            // Parse function body as a block
            ASTNode *func_body = parse_block(p);
            return func_body;  // Return the function body as top-level
        }
        
        // Function declaration only (no body)
        if (match(p, TOK_SEMICOLON)) advance(p);
        return NULL;
    }
    
    // ✅ Rest of variable declaration code (array, assignment, etc.)
    if (match(p, TOK_LBRACKET)) {
        advance(p);
        if (match(p, TOK_NUMBER)) advance(p);
        if (match(p, TOK_RBRACKET)) advance(p);
    }
    
    if (match(p, TOK_ASSIGN)) {
        advance(p);
        ASTNode *assign = create_node(NODE_ASSIGN, "=");
        ASTNode *var = create_node(NODE_VAR, var_name->value);
        ASTNode *expr = parse_expression(p);
        
        if (assign && var) {
            add_child(assign, var);
            if (expr) add_child(assign, expr);
        }
        
        if (match(p, TOK_SEMICOLON)) advance(p);
        return assign;
    }
    
    if (match(p, TOK_SEMICOLON)) advance(p);
    return NULL;
}
    
    if (match(p, TOK_IDENTIFIER)) {
        Token *var_name = tok;
        advance(p);
        
        if (match(p, TOK_LBRACKET)) {
            advance(p);
            ASTNode *index = parse_expression(p);
            if (match(p, TOK_RBRACKET)) advance(p);
            
            if (match(p, TOK_ASSIGN)) {
                advance(p);
                ASTNode *assign = create_node(NODE_ASSIGN, "=");
                ASTNode *arr_access = create_node(NODE_ARRAY_ACCESS, var_name->value);
                if (index) add_child(arr_access, index);
                ASTNode *expr = parse_expression(p);
                
                if (assign && arr_access) {
                    add_child(assign, arr_access);
                    if (expr) add_child(assign, expr);
                }
                
                if (match(p, TOK_SEMICOLON)) advance(p);
                return assign;
            }
        }
        
        if (match(p, TOK_ASSIGN)) {
            advance(p);
            ASTNode *assign = create_node(NODE_ASSIGN, "=");
            ASTNode *var = create_node(NODE_VAR, var_name->value);
            ASTNode *expr = parse_expression(p);
            
            if (assign && var) {
                add_child(assign, var);
                if (expr) add_child(assign, expr);
            }
            
            if (match(p, TOK_SEMICOLON)) advance(p);
            return assign;
        }
        
        if (match(p, TOK_OPERATOR)) {
            Token *op = current_token(p);
            if (strcmp(op->value, "++") == 0 || strcmp(op->value, "--") == 0) {
                advance(p);
                if (match(p, TOK_SEMICOLON)) advance(p);
            }
        }
        
        return NULL;
    }
    
    if (match_keyword(p, "if")) {
        advance(p);
        if (!match(p, TOK_LPAREN)) return NULL;
        advance(p);
        
        ASTNode *if_node = create_node(NODE_IF, "if");
        ASTNode *cond = parse_expression(p);
        if (cond) add_child(if_node, cond);
        
        if (!match(p, TOK_RPAREN)) return NULL;
        advance(p);
        
        ASTNode *then_body = parse_block(p);
        if (then_body) add_child(if_node, then_body);
        
        if (match_keyword(p, "else")) {
            advance(p);
            ASTNode *else_body = parse_block(p);
            if (else_body) add_child(if_node, else_body);
        }
        
        return if_node;
    }
    
    if (match_keyword(p, "while")) {
        advance(p);
        if (!match(p, TOK_LPAREN)) return NULL;
        advance(p);
        
        ASTNode *while_node = create_node(NODE_WHILE, "while");
        ASTNode *cond = parse_expression(p);
        if (cond) add_child(while_node, cond);
        
        if (!match(p, TOK_RPAREN)) return NULL;
        advance(p);
        
        ASTNode *body = parse_block(p);
        if (body) add_child(while_node, body);
        
        return while_node;
    }
    
    if (match_keyword(p, "for")) {
    advance(p);
    if (!match(p, TOK_LPAREN)) return NULL;
    advance(p);

    ASTNode *for_node = create_node(NODE_FOR, "for");
    if (!for_node) return NULL;

    // Parse initialization (can be declaration or assignment)
    ASTNode *init = NULL;
    if (!match(p, TOK_SEMICOLON)) {
        init = parse_statement(p);
    }
    if (match(p, TOK_SEMICOLON)) advance(p);

    // Parse condition
    ASTNode *cond = NULL;
    if (!match(p, TOK_SEMICOLON)) {
        cond = parse_expression(p);
    }
    if (match(p, TOK_SEMICOLON)) advance(p);

    // Parse increment
    ASTNode *inc = NULL;
    if (!match(p, TOK_RPAREN)) {
        inc = parse_expression(p);
    }
    if (match(p, TOK_RPAREN)) advance(p);

    // Parse loop body
    ASTNode *body = parse_block(p);

    if (init) add_child(for_node, init);
    else add_child(for_node, create_node(NODE_LITERAL, "NULL"));

    if (cond) add_child(for_node, cond);
    else add_child(for_node, create_node(NODE_LITERAL, "1"));

    if (inc) add_child(for_node, inc);
    else add_child(for_node, create_node(NODE_LITERAL, "NULL"));

    if (body) add_child(for_node, body);
    else add_child(for_node, create_node(NODE_BLOCK, "empty"));

    return for_node;
}

    
    if (match_keyword(p, "do")) {
        advance(p);
        
        ASTNode *do_while = create_node(NODE_WHILE, "loop");
        ASTNode *body = parse_block(p);
        
        if (match_keyword(p, "while")) {
            advance(p);
            if (match(p, TOK_LPAREN)) advance(p);
            
            ASTNode *cond = parse_expression(p);
            if (cond) add_child(do_while, cond);
            
            if (match(p, TOK_RPAREN)) advance(p);
        }
        
        if (body) add_child(do_while, body);
        if (match(p, TOK_SEMICOLON)) advance(p);
        
        return do_while;
    }
    
    if (match_keyword(p, "switch")) {
        advance(p);
        if (!match(p, TOK_LPAREN)) return NULL;
        advance(p);
        
        ASTNode *switch_node = create_node(NODE_SWITCH, "switch");
        ASTNode *expr = parse_expression(p);
        if (expr) add_child(switch_node, expr);
        
        if (!match(p, TOK_RPAREN)) return NULL;
        advance(p);
        if (!match(p, TOK_LBRACE)) return NULL;
        advance(p);
        
        while (!match(p, TOK_RBRACE) && current_token(p)->type != TOK_EOF) {
            if (match_keyword(p, "case")) {
                advance(p);
                ASTNode *case_node = create_node(NODE_CASE, "case");
                ASTNode *case_val = parse_expression(p);
                if (case_val) add_child(case_node, case_val);
                
                if (match(p, TOK_COLON)) advance(p);
                
                ASTNode *case_body = create_node(NODE_BLOCK, "case_body");
            while (!match_keyword(p, "case") && !match_keyword(p, "default") &&
            !match(p, TOK_RBRACE) && current_token(p)->type != TOK_EOF) {
            if (match_keyword(p, "break")) {
                advance(p);
            if (match(p, TOK_SEMICOLON)) advance(p);
                break;
            }
    ASTNode *stmt = parse_statement(p);
    if (stmt) {
        add_child(case_body, stmt);
    } else {
        // Skip unrecognized tokens in case body
        advance(p);
    }
}
                
                if (case_body) add_child(case_node, case_body);
                if (case_node) add_child(switch_node, case_node);
                
            } else if (match_keyword(p, "default")) {
                advance(p);
                if (match(p, TOK_COLON)) advance(p);
    
            while (!match(p, TOK_RBRACE) && current_token(p)->type != TOK_EOF) {
                if (match_keyword(p, "break")) {
                    advance(p);
                if (match(p, TOK_SEMICOLON)) advance(p);
                break;
            }
        }
            ASTNode *stmt = parse_statement(p);
            if (!stmt) {
                // Skip unrecognized tokens in default case
                advance(p);
            }
        }
    }
        
        if (match(p, TOK_RBRACE)) advance(p);
        return switch_node;
    }
    
    if (match_keyword(p, "return")) {
        advance(p);
        ASTNode *ret = create_node(NODE_RETURN, "return");
        
        if (!match(p, TOK_SEMICOLON)) {
            ASTNode *expr = parse_expression(p);
            if (expr) add_child(ret, expr);
        }
        
        if (match(p, TOK_SEMICOLON)) advance(p);
        return ret;
    }
    
    if (match_keyword(p, "break")) {
        advance(p);
        if (match(p, TOK_SEMICOLON)) advance(p);
        return create_node(NODE_BREAK, "break");
    }
    
    if (match_keyword(p, "continue")) {
        advance(p);
        if (match(p, TOK_SEMICOLON)) advance(p);
        return create_node(NODE_CONTINUE, "continue");
    }
    
    if (match(p, TOK_LBRACE)) {
        return parse_block(p);
    }
    
    return NULL;
    }

ASTNode* parse(TokenList *tokens) {
 if (!tokens || tokens->count == 0) return NULL;
    
    Parser p = {tokens->tokens, 0, tokens->count};
    ASTNode *root = create_node(NODE_PROGRAM, "program");
    if (!root) return NULL;
    
    while (current_token(&p)->type != TOK_EOF) {
        ASTNode *stmt = parse_statement(&p);
        if (stmt) {
            add_child(root, stmt);
        } else {
            advance(&p);
        }
    }
    return root;
}