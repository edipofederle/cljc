#include <stdio.h>
#include "tokenizer.h"
#include "parser.h"
#include "codegen.h"

int main(int argc, char *argv[]) {
    const char *source_code;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s \"<clojure code>\"\n", argv[0]);
        fprintf(stderr, "Example: %s \"(+ 1 2 3)\"\n", argv[0]);
        return 1;
    }

    source_code = argv[1];

    printf("Mini Clojure Compiler\n");
    printf("=====================\n\n");
    printf("Source: %s\n\n", source_code);

    TokenList *tokens = tokenize(source_code);
    printf("Tokens:\n");
    print_tokens(tokens);

    printf("\nAST:\n");
    ASTNode *ast = parse(tokens);
    if (ast) {
        print_ast(ast, 0);

        printf("\nGenerating ARM64 assembly...\n");
        generate_asm(ast, "asm/output.s");
        printf("Assembly written to: asm/output.s\n");

        free_ast(ast);
    } else {
        printf("Failed to parse!\n");
        return 1;
    }

    free_tokens(tokens);
    return 0;
}
