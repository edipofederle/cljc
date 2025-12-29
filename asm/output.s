    .section __TEXT,__text,regular,pure_instructions
    .build_version macos, 14, 0
    .p2align 2

    .globl _make_token
_make_token:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    // Save parameters to stack
    str d0, [sp, #-16]!
    str d1, [sp, #-16]!
    str d2, [sp, #-16]!
    str d3, [sp, #-16]!
    // Generate function body
    // List function: cons
    // Load parameter
    ldr d0, [x29, #-64]
    str d0, [sp, #-16]!
    // List function: cons
    // Load parameter
    ldr d0, [x29, #-48]
    str d0, [sp, #-16]!
    // List function: cons
    // Load parameter
    ldr d0, [x29, #-32]
    str d0, [sp, #-16]!
    // List function: cons
    // Load parameter
    ldr d0, [x29, #-16]
    str d0, [sp, #-16]!
    // List function: empty-list
    bl _create_list
    str x0, [sp, #-16]!
    ldr x1, [sp], #16
    ldr d0, [sp], #16
    bl _cons
    str x0, [sp, #-16]!
    ldr x1, [sp], #16
    ldr d0, [sp], #16
    bl _cons
    str x0, [sp, #-16]!
    ldr x1, [sp], #16
    ldr d0, [sp], #16
    bl _cons
    str x0, [sp, #-16]!
    ldr x1, [sp], #16
    ldr d0, [sp], #16
    bl _cons
    str x0, [sp, #-16]!
    // Pop result into d0
    ldr d0, [sp], #16
    mov sp, x29
    ldp x29, x30, [sp], #16
    ret

    .globl _tokenize_poc
_tokenize_poc:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    // Save parameters to stack
    // Generate function body
    // List function: append
    // List function: append
    // List function: empty-list
    bl _create_list
    str x0, [sp, #-16]!
    // Call function: make-token
    // Load number
    adrp x8, .L_const_0@PAGE
    add x8, x8, .L_const_0@PAGEOFF
    ldr d0, [x8]
    str d0, [sp, #-16]!
    // Load number
    adrp x8, .L_const_1@PAGE
    add x8, x8, .L_const_1@PAGEOFF
    ldr d0, [x8]
    str d0, [sp, #-16]!
    // Load number
    adrp x8, .L_const_2@PAGE
    add x8, x8, .L_const_2@PAGEOFF
    ldr d0, [x8]
    str d0, [sp, #-16]!
    // Load number
    adrp x8, .L_const_2@PAGE
    add x8, x8, .L_const_2@PAGEOFF
    ldr d0, [x8]
    str d0, [sp, #-16]!
    ldr d3, [sp], #16
    ldr d2, [sp], #16
    ldr d1, [sp], #16
    ldr d0, [sp], #16
    bl _make_token
    str d0, [sp, #-16]!
    ldr d1, [sp], #16
    ldr x0, [sp], #16
    bl _append_elem
    str x0, [sp, #-16]!
    // Call function: make-token
    // Load number
    adrp x8, .L_const_3@PAGE
    add x8, x8, .L_const_3@PAGEOFF
    ldr d0, [x8]
    str d0, [sp, #-16]!
    // Load number
    adrp x8, .L_const_4@PAGE
    add x8, x8, .L_const_4@PAGEOFF
    ldr d0, [x8]
    str d0, [sp, #-16]!
    // Load number
    adrp x8, .L_const_2@PAGE
    add x8, x8, .L_const_2@PAGEOFF
    ldr d0, [x8]
    str d0, [sp, #-16]!
    // Load number
    adrp x8, .L_const_5@PAGE
    add x8, x8, .L_const_5@PAGEOFF
    ldr d0, [x8]
    str d0, [sp, #-16]!
    ldr d3, [sp], #16
    ldr d2, [sp], #16
    ldr d1, [sp], #16
    ldr d0, [sp], #16
    bl _make_token
    str d0, [sp, #-16]!
    ldr d1, [sp], #16
    ldr x0, [sp], #16
    bl _append_elem
    str x0, [sp, #-16]!
    // Pop result into d0
    ldr d0, [sp], #16
    mov sp, x29
    ldp x29, x30, [sp], #16
    ret

    .section __TEXT,__text
    .p2align 2
    .globl _main
_main:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    // Evaluate expression
    // List function: list-count
    // Call function: tokenize-poc
    bl _tokenize_poc
    str d0, [sp, #-16]!
    ldr x0, [sp], #16
    bl _list_count
    str d0, [sp, #-16]!
    // Pop result and print
    ldr d0, [sp], #16
    bl _print_double
    // Return 0
    mov w0, #0
    mov sp, x29
    ldp x29, x30, [sp], #16
    ret

    .section __DATA,__data
    .p2align 3
.L_const_0:
    .double 0.000000
.L_const_1:
    .double 40.000000
.L_const_2:
    .double 1.000000
.L_const_3:
    .double 5.000000
.L_const_4:
    .double 43.000000
.L_const_5:
    .double 2.000000
