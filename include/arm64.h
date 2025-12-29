#ifndef ARM64_H
#define ARM64_H

#include <stdio.h>

void emit_header(FILE *f);
void emit_data_section_start(FILE *f);
void emit_text_section_start(FILE *f);
void emit_function_start(FILE *f, const char *name);
void emit_function_prologue(FILE *f);
void emit_function_epilogue(FILE *f);
void emit_load_double_literal(FILE *f, const char *label);
void emit_push_double(FILE *f, int dreg);
void emit_pop_double(FILE *f, int dreg);
void emit_fadd(FILE *f);
void emit_fsub(FILE *f);
void emit_fmul(FILE *f);
void emit_fdiv(FILE *f);
void emit_call(FILE *f, const char *label);
void emit_return(FILE *f);
void emit_comment(FILE *f, const char *comment);
void emit_float_constant(FILE *f, const char *label, double value);
void emit_fcmp(FILE *f);
void emit_label(FILE *f, const char *label);
void emit_branch(FILE *f, const char *label);
void emit_branch_eq(FILE *f, const char *label);
void emit_branch_ne(FILE *f, const char *label);
void emit_branch_lt(FILE *f, const char *label);
void emit_branch_le(FILE *f, const char *label);
void emit_branch_gt(FILE *f, const char *label);
void emit_branch_ge(FILE *f, const char *label);
void emit_cset(FILE *f, int reg, const char *condition);

#endif
