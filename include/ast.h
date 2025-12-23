#ifndef AST_H
#define AST_H

typedef enum {
    AST_NUMBER,
    AST_SYMBOL,
    AST_LIST
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        char *symbol;
        double number;
        struct {
            struct ASTNode **elements;
            int count;
            int capacity;
        } list;
    } as;
} ASTNode;

ASTNode *create_number_node(double value);
ASTNode *create_symbol_node(const char *symbol);
ASTNode *create_list_node(void);
void add_to_list(ASTNode *list, ASTNode *element);
void free_ast(ASTNode *node);
void print_ast(ASTNode *node, int indent);

#endif
