#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"
#include "ast.h"

typedef struct {
    TokenList *tokens;
    int current;
} Parser;

ASTNode *parse(TokenList *tokens);

#endif
