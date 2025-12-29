#include "arm64.h"

void emit_header(FILE *f) {
    fprintf(f, "    .section __TEXT,__text,regular,pure_instructions\n");
    fprintf(f, "    .build_version macos, 14, 0\n");
    fprintf(f, "    .p2align 2\n");
}

void emit_data_section_start(FILE *f) {
    fprintf(f, "\n    .section __DATA,__data\n");
    fprintf(f, "    .p2align 3\n");
}

void emit_text_section_start(FILE *f) {
    fprintf(f, "\n    .section __TEXT,__text\n");
    fprintf(f, "    .p2align 2\n");
}

void emit_function_start(FILE *f, const char *name) {
    fprintf(f, "    .globl %s\n", name);
    fprintf(f, "%s:\n", name);
}

void emit_function_prologue(FILE *f) {
    fprintf(f, "    stp x29, x30, [sp, #-16]!\n");
    fprintf(f, "    mov x29, sp\n");
}

void emit_function_epilogue(FILE *f) {
    fprintf(f, "    mov sp, x29\n");
    fprintf(f, "    ldp x29, x30, [sp], #16\n");
    fprintf(f, "    ret\n");
}

void emit_load_double_literal(FILE *f, const char *label) {
    fprintf(f, "    adrp x8, %s@PAGE\n", label);
    fprintf(f, "    add x8, x8, %s@PAGEOFF\n", label);
    fprintf(f, "    ldr d0, [x8]\n");
}

void emit_push_double(FILE *f, int dreg) {
    fprintf(f, "    str d%d, [sp, #-16]!\n", dreg);
}

void emit_pop_double(FILE *f, int dreg) {
    fprintf(f, "    ldr d%d, [sp], #16\n", dreg);
}

void emit_fadd(FILE *f) {
    fprintf(f, "    fadd d0, d0, d1\n");
}

void emit_fsub(FILE *f) {
    fprintf(f, "    fsub d0, d1, d0\n");
}

void emit_fmul(FILE *f) {
    fprintf(f, "    fmul d0, d0, d1\n");
}

void emit_fdiv(FILE *f) {
    fprintf(f, "    fdiv d0, d1, d0\n");
}

void emit_call(FILE *f, const char *label) {
    fprintf(f, "    bl %s\n", label);
}

void emit_return(FILE *f) {
    fprintf(f, "    ret\n");
}

void emit_comment(FILE *f, const char *comment) {
    fprintf(f, "    // %s\n", comment);
}

void emit_float_constant(FILE *f, const char *label, double value) {
    fprintf(f, "%s:\n", label);
    fprintf(f, "    .double %f\n", value);
}

void emit_string_constant(FILE *f, const char *label, const char *value) {
    fprintf(f, "%s:\n", label);
    fprintf(f, "    .asciz \"%s\"\n", value);
}

void emit_fcmp(FILE *f) {
    fprintf(f, "    fcmp d0, d1\n");
}

void emit_label(FILE *f, const char *label) {
    fprintf(f, "%s:\n", label);
}

void emit_branch(FILE *f, const char *label) {
    fprintf(f, "    b %s\n", label);
}

void emit_branch_eq(FILE *f, const char *label) {
    fprintf(f, "    b.eq %s\n", label);
}

void emit_branch_ne(FILE *f, const char *label) {
    fprintf(f, "    b.ne %s\n", label);
}

void emit_branch_lt(FILE *f, const char *label) {
    fprintf(f, "    b.lt %s\n", label);
}

void emit_branch_le(FILE *f, const char *label) {
    fprintf(f, "    b.le %s\n", label);
}

void emit_branch_gt(FILE *f, const char *label) {
    fprintf(f, "    b.gt %s\n", label);
}

void emit_branch_ge(FILE *f, const char *label) {
    fprintf(f, "    b.ge %s\n", label);
}

void emit_cset(FILE *f, int reg, const char *condition) {
    fprintf(f, "    cset x%d, %s\n", reg, condition);
}
