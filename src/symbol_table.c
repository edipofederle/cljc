#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

#define INITIAL_CAPACITY 8

SymbolTable* create_symbol_table(void) {
    SymbolTable *table = malloc(sizeof(SymbolTable));
    table->function_capacity = INITIAL_CAPACITY;
    table->function_count = 0;
    table->functions = malloc(INITIAL_CAPACITY * sizeof(FunctionInfo *));
    return table;
}

void add_function(SymbolTable *table, const char *name, int arity,
                  char **params, ASTNode *body) {
    if (table->function_count >= table->function_capacity) {
        table->function_capacity *= 2;
        table->functions = realloc(table->functions,
                                   table->function_capacity * sizeof(FunctionInfo *));
    }

    FunctionInfo *func = malloc(sizeof(FunctionInfo));
    func->name = strdup(name);
    func->arity = arity;
    func->body = body;

    func->param_names = malloc(arity * sizeof(char *));
    for (int i = 0; i < arity; i++) {
        func->param_names[i] = strdup(params[i]);
    }

    char *label = malloc(strlen(name) + 2);
    sprintf(label, "_%s", name);
    for (char *p = label; *p; p++) {
        if (*p == '-') *p = '_';
    }
    func->label = label;

    table->functions[table->function_count++] = func;
}

FunctionInfo* lookup_function(SymbolTable *table, const char *name) {
    for (int i = 0; i < table->function_count; i++) {
        if (strcmp(table->functions[i]->name, name) == 0) {
            return table->functions[i];
        }
    }
    return NULL;
}

void free_symbol_table(SymbolTable *table) {
    for (int i = 0; i < table->function_count; i++) {
        FunctionInfo *func = table->functions[i];
        free(func->name);
        free(func->label);
        for (int j = 0; j < func->arity; j++) {
            free(func->param_names[j]);
        }
        free(func->param_names);
        free(func);
    }
    free(table->functions);
    free(table);
}

void print_symbol_table(SymbolTable *table) {
    printf("Symbol Table (%d functions):\n", table->function_count);
    for (int i = 0; i < table->function_count; i++) {
        FunctionInfo *func = table->functions[i];
        printf("  %s (arity: %d, label: %s)\n", func->name, func->arity, func->label);
        printf("    Params:");
        for (int j = 0; j < func->arity; j++) {
            printf(" %s", func->param_names[j]);
        }
        printf("\n");
    }
}
