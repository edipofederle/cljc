#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "ast.h"

typedef struct FunctionInfo {
    char *name;
    int arity;
    char **param_names;
    ASTNode *body;
    char *label;
} FunctionInfo;

typedef struct SymbolTable {
    FunctionInfo **functions;
    int function_count;
    int function_capacity;
} SymbolTable;

SymbolTable* create_symbol_table(void);
void add_function(SymbolTable *table, const char *name, int arity,
                  char **params, ASTNode *body);
FunctionInfo* lookup_function(SymbolTable *table, const char *name);
void free_symbol_table(SymbolTable *table);
void print_symbol_table(SymbolTable *table);

#endif
