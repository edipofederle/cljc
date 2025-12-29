#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

#define INITIAL_LIST_CAPACITY 8

ASTNode *create_number_node(double value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_NUMBER;
    node->as.number = value;
    return node;
}

ASTNode *create_symbol_node(const char *symbol) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_SYMBOL;
    node->as.symbol = strdup(symbol);
    return node;
}

ASTNode *create_string_node(const char *string) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_STRING;
    node->as.string = strdup(string);
    return node;
}

ASTNode *create_list_node(void) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_LIST;
    node->as.list.capacity = INITIAL_LIST_CAPACITY;
    node->as.list.count = 0;
    node->as.list.elements = malloc(INITIAL_LIST_CAPACITY * sizeof(ASTNode *));
    return node;
}

void add_to_list(ASTNode *list, ASTNode *element) {
    if (list->type != AST_LIST) {
        fprintf(stderr, "Error: Trying to add element to non-list node\n");
        return;
    }

    if (list->as.list.count >= list->as.list.capacity) {
        list->as.list.capacity *= 2;
        list->as.list.elements = realloc(list->as.list.elements,
                                         list->as.list.capacity * sizeof(ASTNode *));
    }

    list->as.list.elements[list->as.list.count++] = element;
}

void free_ast(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case AST_SYMBOL:
            free(node->as.symbol);
            break;
        case AST_STRING:
            free(node->as.string);
            break;
        case AST_LIST:
            for (int i = 0; i < node->as.list.count; i++) {
                free_ast(node->as.list.elements[i]);
            }
            free(node->as.list.elements);
            break;
        case AST_NUMBER:
            break;
    }

    free(node);
}

static void print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
}

void print_ast(ASTNode *node, int indent) {
    if (!node) {
        print_indent(indent);
        printf("(null)\n");
        return;
    }

    switch (node->type) {
        case AST_NUMBER:
            print_indent(indent);
            printf("Number: %.2f\n", node->as.number);
            break;

        case AST_SYMBOL:
            print_indent(indent);
            printf("Symbol: %s\n", node->as.symbol);
            break;

        case AST_STRING:
            print_indent(indent);
            printf("String: \"%s\"\n", node->as.string);
            break;

        case AST_LIST:
            print_indent(indent);
            printf("List (%d elements):\n", node->as.list.count);
            for (int i = 0; i < node->as.list.count; i++) {
                print_ast(node->as.list.elements[i], indent + 1);
            }
            break;
    }
}
