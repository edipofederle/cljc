#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include "symbol_table.h"
#include <stdio.h>

typedef struct FloatConstant {
    double value;
    char *label;
} FloatConstant;

typedef struct StringConstant {
    char *value;
    char *label;
} StringConstant;

typedef struct Variable {
    char *name;
    double value;
    char *label;
} Variable;

typedef struct CodeGen {
    FILE *output;
    SymbolTable *symbols;
    int label_counter;
    FloatConstant **float_constants;
    int float_count;
    int float_capacity;
    StringConstant **string_constants;
    int string_count;
    int string_capacity;
    Variable **variables;
    int var_count;
    int var_capacity;
} CodeGen;

void generate_asm(ASTNode *ast, const char *output_file);

#endif
