#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "arm64.h"

#define INITIAL_FLOAT_CAPACITY 16
#define INITIAL_VAR_CAPACITY 16

static void init_codegen(CodeGen *cg, const char *output_file) {
    cg->output = fopen(output_file, "w");
    if (!cg->output) {
        fprintf(stderr, "Error: Could not open output file: %s\n", output_file);
        exit(1);
    }
    cg->symbols = create_symbol_table();
    cg->label_counter = 0;
    cg->float_capacity = INITIAL_FLOAT_CAPACITY;
    cg->float_count = 0;
    cg->float_constants = malloc(INITIAL_FLOAT_CAPACITY * sizeof(FloatConstant *));
    cg->var_capacity = INITIAL_VAR_CAPACITY;
    cg->var_count = 0;
    cg->variables = malloc(INITIAL_VAR_CAPACITY * sizeof(Variable *));
}

static void cleanup_codegen(CodeGen *cg) {
    for (int i = 0; i < cg->float_count; i++) {
        free(cg->float_constants[i]->label);
        free(cg->float_constants[i]);
    }
    free(cg->float_constants);
    for (int i = 0; i < cg->var_count; i++) {
        free(cg->variables[i]->name);
        free(cg->variables[i]->label);
        free(cg->variables[i]);
    }
    free(cg->variables);
    free_symbol_table(cg->symbols);
    fclose(cg->output);
}

static const char* add_float_constant(CodeGen *cg, double value) {
    for (int i = 0; i < cg->float_count; i++) {
        if (cg->float_constants[i]->value == value) {
            return cg->float_constants[i]->label;
        }
    }

    if (cg->float_count >= cg->float_capacity) {
        cg->float_capacity *= 2;
        cg->float_constants = realloc(cg->float_constants,
                                      cg->float_capacity * sizeof(FloatConstant *));
    }

    FloatConstant *fc = malloc(sizeof(FloatConstant));
    fc->value = value;
    fc->label = malloc(32);
    sprintf(fc->label, ".L_const_%d", cg->label_counter++);

    cg->float_constants[cg->float_count++] = fc;
    return fc->label;
}

static void add_variable(CodeGen *cg, const char *name, double value) {
    if (cg->var_count >= cg->var_capacity) {
        cg->var_capacity *= 2;
        cg->variables = realloc(cg->variables,
                               cg->var_capacity * sizeof(Variable *));
    }

    Variable *var = malloc(sizeof(Variable));
    var->name = strdup(name);
    var->value = value;
    var->label = malloc(32);
    sprintf(var->label, ".L_var_%s", name);

    cg->variables[cg->var_count++] = var;
}

static Variable* lookup_variable(CodeGen *cg, const char *name) {
    for (int i = 0; i < cg->var_count; i++) {
        if (strcmp(cg->variables[i]->name, name) == 0) {
            return cg->variables[i];
        }
    }
    return NULL;
}

static void emit_data_section(CodeGen *cg) {
    if (cg->float_count > 0 || cg->var_count > 0) {
        emit_data_section_start(cg->output);
        for (int i = 0; i < cg->float_count; i++) {
            emit_float_constant(cg->output,
                              cg->float_constants[i]->label,
                              cg->float_constants[i]->value);
        }
        for (int i = 0; i < cg->var_count; i++) {
            emit_float_constant(cg->output,
                              cg->variables[i]->label,
                              cg->variables[i]->value);
        }
    }
}

static void generate_expr(CodeGen *cg, ASTNode *node);

static void generate_number(CodeGen *cg, double value) {
    const char *label = add_float_constant(cg, value);
    emit_comment(cg->output, "Load number");
    emit_load_double_literal(cg->output, label);
    emit_push_double(cg->output, 0);
}

static int is_operator(const char *symbol) {
    return strcmp(symbol, "+") == 0 ||
           strcmp(symbol, "-") == 0 ||
           strcmp(symbol, "*") == 0 ||
           strcmp(symbol, "/") == 0;
}

static int is_comparison(const char *symbol) {
    return strcmp(symbol, "<") == 0 ||
           strcmp(symbol, ">") == 0 ||
           strcmp(symbol, "=") == 0 ||
           strcmp(symbol, "<=") == 0 ||
           strcmp(symbol, ">=") == 0;
}

static void generate_operator(CodeGen *cg, const char *op, ASTNode **args, int arg_count) {
    if (arg_count == 0) {
        fprintf(stderr, "Error: Operator %s requires at least one argument\n", op);
        exit(1);
    }

    char comment[64];
    snprintf(comment, sizeof(comment), "Operator: %s", op);
    emit_comment(cg->output, comment);

    for (int i = 0; i < arg_count; i++) {
        generate_expr(cg, args[i]);
    }

    emit_pop_double(cg->output, 0);

    for (int i = 1; i < arg_count; i++) {
        emit_pop_double(cg->output, 1);

        if (strcmp(op, "+") == 0) {
            emit_fadd(cg->output);
        } else if (strcmp(op, "-") == 0) {
            emit_fsub(cg->output);
        } else if (strcmp(op, "*") == 0) {
            emit_fmul(cg->output);
        } else if (strcmp(op, "/") == 0) {
            emit_fdiv(cg->output);
        }
    }

    emit_push_double(cg->output, 0);
}

static void generate_comparison(CodeGen *cg, const char *op, ASTNode **args, int arg_count) {
    if (arg_count != 2) {
        fprintf(stderr, "Error: Comparison operator %s requires exactly 2 arguments\n", op);
        exit(1);
    }

    char comment[64];
    snprintf(comment, sizeof(comment), "Comparison: %s", op);
    emit_comment(cg->output, comment);

    generate_expr(cg, args[0]);
    generate_expr(cg, args[1]);

    emit_pop_double(cg->output, 1);
    emit_pop_double(cg->output, 0);

    emit_fcmp(cg->output);

    const char *condition;
    if (strcmp(op, "<") == 0) {
        condition = "lt";
    } else if (strcmp(op, ">") == 0) {
        condition = "gt";
    } else if (strcmp(op, "=") == 0) {
        condition = "eq";
    } else if (strcmp(op, "<=") == 0) {
        condition = "le";
    } else if (strcmp(op, ">=") == 0) {
        condition = "ge";
    } else {
        fprintf(stderr, "Error: Unknown comparison operator: %s\n", op);
        exit(1);
    }

    emit_cset(cg->output, 0, condition);
    fprintf(cg->output, "    ucvtf d0, x0\n");
    emit_push_double(cg->output, 0);
}

static void generate_if(CodeGen *cg, ASTNode **args, int arg_count) {
    if (arg_count != 3) {
        fprintf(stderr, "Error: if requires exactly 3 arguments (condition then else)\n");
        exit(1);
    }

    char else_label[32];
    char end_label[32];
    sprintf(else_label, ".L_else_%d", cg->label_counter);
    sprintf(end_label, ".L_end_%d", cg->label_counter);
    cg->label_counter++;

    emit_comment(cg->output, "If: evaluate condition");
    generate_expr(cg, args[0]);

    emit_comment(cg->output, "If: check condition");
    emit_pop_double(cg->output, 0);
    fprintf(cg->output, "    fcmp d0, #0.0\n");
    emit_branch_eq(cg->output, else_label);

    emit_comment(cg->output, "If: then branch");
    generate_expr(cg, args[1]);
    emit_branch(cg->output, end_label);

    emit_comment(cg->output, "If: else branch");
    emit_label(cg->output, else_label);
    generate_expr(cg, args[2]);

    emit_label(cg->output, end_label);
}

static void generate_function_call(CodeGen *cg, const char *name, ASTNode **args, int arg_count) {
    FunctionInfo *func = lookup_function(cg->symbols, name);
    if (!func) {
        fprintf(stderr, "Error: Undefined function: %s\n", name);
        exit(1);
    }

    if (func->arity != arg_count) {
        fprintf(stderr, "Error: Function %s expects %d arguments, got %d\n",
                name, func->arity, arg_count);
        exit(1);
    }

    char comment[64];
    snprintf(comment, sizeof(comment), "Call function: %s", name);
    emit_comment(cg->output, comment);

    for (int i = 0; i < arg_count; i++) {
        generate_expr(cg, args[i]);
    }

    for (int i = arg_count - 1; i >= 0 && i < 8; i--) {
        emit_pop_double(cg->output, i);
    }

    emit_call(cg->output, func->label);
    emit_push_double(cg->output, 0);
}

static void generate_list(CodeGen *cg, ASTNode *node) {
    if (node->as.list.count == 0) {
        fprintf(stderr, "Error: Empty list not allowed\n");
        exit(1);
    }

    ASTNode *first = node->as.list.elements[0];
    if (first->type != AST_SYMBOL) {
        fprintf(stderr, "Error: First element of list must be a symbol\n");
        exit(1);
    }

    const char *symbol = first->as.symbol;
    ASTNode **args = &node->as.list.elements[1];
    int arg_count = node->as.list.count - 1;

    if (is_operator(symbol)) {
        generate_operator(cg, symbol, args, arg_count);
    } else if (is_comparison(symbol)) {
        generate_comparison(cg, symbol, args, arg_count);
    } else if (strcmp(symbol, "if") == 0) {
        generate_if(cg, args, arg_count);
    } else if (strcmp(symbol, "defn") == 0) {
        fprintf(stderr, "Error: defn not yet supported in this context\n");
        exit(1);
    } else {
        generate_function_call(cg, symbol, args, arg_count);
    }
}

typedef struct {
    char **param_names;
    int param_count;
} LocalContext;

static LocalContext *current_context = NULL;

static int find_param_index(const char *name) {
    if (!current_context) return -1;

    for (int i = 0; i < current_context->param_count; i++) {
        if (strcmp(current_context->param_names[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

static void generate_symbol(CodeGen *cg, const char *symbol) {
    Variable *var = lookup_variable(cg, symbol);
    if (var) {
        emit_comment(cg->output, "Load variable");
        fprintf(cg->output, "    adrp x0, %s@PAGE\n", var->label);
        fprintf(cg->output, "    add x0, x0, %s@PAGEOFF\n", var->label);
        fprintf(cg->output, "    ldr d0, [x0]\n");
        emit_push_double(cg->output, 0);
        return;
    }

    int param_idx = find_param_index(symbol);
    if (param_idx < 0) {
        fprintf(stderr, "Error: Undefined symbol: %s\n", symbol);
        exit(1);
    }

    emit_comment(cg->output, "Load parameter");
    int offset = -(current_context->param_count - param_idx) * 16;
    fprintf(cg->output, "    ldr d0, [x29, #%d]\n", offset);
    emit_push_double(cg->output, 0);
}

static void generate_expr(CodeGen *cg, ASTNode *node) {
    switch (node->type) {
        case AST_NUMBER:
            generate_number(cg, node->as.number);
            break;

        case AST_SYMBOL:
            generate_symbol(cg, node->as.symbol);
            break;

        case AST_LIST:
            generate_list(cg, node);
            break;
    }
}

static int is_defn(ASTNode *node) {
    return node->type == AST_LIST &&
           node->as.list.count > 0 &&
           node->as.list.elements[0]->type == AST_SYMBOL &&
           strcmp(node->as.list.elements[0]->as.symbol, "defn") == 0;
}

static int is_def(ASTNode *node) {
    return node->type == AST_LIST &&
           node->as.list.count == 3 &&
           node->as.list.elements[0]->type == AST_SYMBOL &&
           strcmp(node->as.list.elements[0]->as.symbol, "def") == 0;
}

static int is_top_level_container(ASTNode *node) {
    if (node->type != AST_LIST || node->as.list.count == 0) {
        return 0;
    }

    for (int i = 0; i < node->as.list.count; i++) {
        if (is_defn(node->as.list.elements[i]) || is_def(node->as.list.elements[i])) {
            return 1;
        }
    }

    if (node->as.list.elements[0]->type != AST_SYMBOL) {
        return 0;
    }

    return 0;
}

static void generate_main(CodeGen *cg, ASTNode *ast) {
    emit_text_section_start(cg->output);
    emit_function_start(cg->output, "_main");
    emit_function_prologue(cg->output);

    if (is_top_level_container(ast)) {
        for (int i = 0; i < ast->as.list.count; i++) {
            if (!is_defn(ast->as.list.elements[i]) && !is_def(ast->as.list.elements[i])) {
                emit_comment(cg->output, "Evaluate expression");
                generate_expr(cg, ast->as.list.elements[i]);

                emit_comment(cg->output, "Pop result and print");
                emit_pop_double(cg->output, 0);
                emit_call(cg->output, "_print_double");
            }
        }
    } else if (!is_defn(ast) && !is_def(ast)) {
        emit_comment(cg->output, "Evaluate expression");
        generate_expr(cg, ast);

        emit_comment(cg->output, "Pop result and print");
        emit_pop_double(cg->output, 0);
        emit_call(cg->output, "_print_double");
    }

    emit_comment(cg->output, "Return 0");
    fprintf(cg->output, "    mov w0, #0\n");
    emit_function_epilogue(cg->output);
}

static void collect_functions_from_node(CodeGen *cg, ASTNode *node) {
    if (is_defn(node)) {
        if (node->as.list.count < 4) {
            fprintf(stderr, "Error: defn requires at least 3 arguments\n");
            exit(1);
        }

        ASTNode *name_node = node->as.list.elements[1];
        ASTNode *params_node = node->as.list.elements[2];
        ASTNode *body_node = node->as.list.elements[3];

        if (name_node->type != AST_SYMBOL) {
            fprintf(stderr, "Error: Function name must be a symbol\n");
            exit(1);
        }

        if (params_node->type != AST_LIST) {
            fprintf(stderr, "Error: Function parameters must be a list\n");
            exit(1);
        }

        char **param_names = malloc(params_node->as.list.count * sizeof(char *));
        for (int i = 0; i < params_node->as.list.count; i++) {
            if (params_node->as.list.elements[i]->type != AST_SYMBOL) {
                fprintf(stderr, "Error: Parameter names must be symbols\n");
                exit(1);
            }
            param_names[i] = params_node->as.list.elements[i]->as.symbol;
        }

        add_function(cg->symbols, name_node->as.symbol,
                    params_node->as.list.count, param_names, body_node);

        free(param_names);
    } else if (is_def(node)) {
        ASTNode *name_node = node->as.list.elements[1];
        ASTNode *value_node = node->as.list.elements[2];

        if (name_node->type != AST_SYMBOL) {
            fprintf(stderr, "Error: Variable name must be a symbol\n");
            exit(1);
        }

        if (value_node->type != AST_NUMBER) {
            fprintf(stderr, "Error: Variable value must be a number\n");
            exit(1);
        }

        add_variable(cg, name_node->as.symbol, value_node->as.number);
    }
}

static void collect_functions(CodeGen *cg, ASTNode *ast) {
    if (ast->type == AST_LIST && !is_defn(ast) && !is_def(ast)) {
        for (int i = 0; i < ast->as.list.count; i++) {
            collect_functions_from_node(cg, ast->as.list.elements[i]);
        }
    } else {
        collect_functions_from_node(cg, ast);
    }
}

static void generate_function(CodeGen *cg, FunctionInfo *func) {
    fprintf(cg->output, "\n");
    emit_function_start(cg->output, func->label);
    emit_function_prologue(cg->output);

    emit_comment(cg->output, "Save parameters to stack");
    for (int i = 0; i < func->arity && i < 8; i++) {
        fprintf(cg->output, "    str d%d, [sp, #-16]!\n", i);
    }

    LocalContext ctx;
    ctx.param_names = func->param_names;
    ctx.param_count = func->arity;
    current_context = &ctx;

    emit_comment(cg->output, "Generate function body");
    generate_expr(cg, func->body);

    current_context = NULL;

    emit_comment(cg->output, "Pop result into d0");
    emit_pop_double(cg->output, 0);

    emit_function_epilogue(cg->output);
}

static void generate_user_functions(CodeGen *cg) {
    for (int i = 0; i < cg->symbols->function_count; i++) {
        generate_function(cg, cg->symbols->functions[i]);
    }
}

void generate_asm(ASTNode *ast, const char *output_file) {
    CodeGen cg;
    init_codegen(&cg, output_file);

    collect_functions(&cg, ast);

    emit_header(cg.output);
    generate_user_functions(&cg);
    generate_main(&cg, ast);
    emit_data_section(&cg);

    cleanup_codegen(&cg);
}
