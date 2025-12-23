#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

static Token *peek(Parser *p) {
    if (p->current >= p->tokens->count) {
        return NULL;
    }
    return &p->tokens->tokens[p->current];
}

static Token *advance(Parser *p) {
    if (p->current >= p->tokens->count) {
        return NULL;
    }
    return &p->tokens->tokens[p->current++];
}

static int match(Parser *p, TokenType type) {
    Token *token = peek(p);
    return token && token->type == type;
}

static ASTNode *parse_expression(Parser *p);

static ASTNode *parse_list(Parser *p) {
    Token *lparen = advance(p);
    if (!lparen || lparen->type != TOKEN_LEFT_PAREN) {
        fprintf(stderr, "Error: Expected '(' at line %d, column %d\n",
                lparen ? lparen->line : 0, lparen ? lparen->column : 0);
        return NULL;
    }

    ASTNode *list = create_list_node();

    while (!match(p, TOKEN_RIGHT_PAREN) && !match(p, TOKEN_EOF)) {
        ASTNode *element = parse_expression(p);
        if (!element) {
            free_ast(list);
            return NULL;
        }
        add_to_list(list, element);
    }

    Token *rparen = advance(p);
    if (!rparen || rparen->type != TOKEN_RIGHT_PAREN) {
        fprintf(stderr, "Error: Expected ')' at line %d, column %d\n",
                rparen ? rparen->line : 0, rparen ? rparen->column : 0);
        free_ast(list);
        return NULL;
    }

    return list;
}

static ASTNode *parse_vector(Parser *p) {
    Token *lbracket = advance(p);
    if (!lbracket || lbracket->type != TOKEN_LEFT_BRACKET) {
        fprintf(stderr, "Error: Expected '[' at line %d, column %d\n",
                lbracket ? lbracket->line : 0, lbracket ? lbracket->column : 0);
        return NULL;
    }

    ASTNode *list = create_list_node();

    while (!match(p, TOKEN_RIGHT_BRACKET) && !match(p, TOKEN_EOF)) {
        ASTNode *element = parse_expression(p);
        if (!element) {
            free_ast(list);
            return NULL;
        }
        add_to_list(list, element);
    }

    Token *rbracket = advance(p);
    if (!rbracket || rbracket->type != TOKEN_RIGHT_BRACKET) {
        fprintf(stderr, "Error: Expected ']' at line %d, column %d\n",
                rbracket ? rbracket->line : 0, rbracket ? rbracket->column : 0);
        free_ast(list);
        return NULL;
    }

    return list;
}

static ASTNode *parse_expression(Parser *p) {
    Token *token = peek(p);

    if (!token) {
        fprintf(stderr, "Error: Unexpected end of input\n");
        return NULL;
    }

    switch (token->type) {
        case TOKEN_LEFT_PAREN:
            return parse_list(p);

        case TOKEN_LEFT_BRACKET:
            return parse_vector(p);

        case TOKEN_NUMBER: {
            advance(p);
            double value = atof(token->value);
            return create_number_node(value);
        }

        case TOKEN_SYMBOL: {
            advance(p);
            return create_symbol_node(token->value);
        }

        case TOKEN_EOF:
            return NULL;

        case TOKEN_ERROR:
            fprintf(stderr, "Error: %s at line %d, column %d\n",
                    token->value, token->line, token->column);
            return NULL;

        default:
            fprintf(stderr, "Error: Unexpected token type at line %d, column %d\n",
                    token->line, token->column);
            return NULL;
    }
}

ASTNode *parse(TokenList *tokens) {
    Parser parser;
    parser.tokens = tokens;
    parser.current = 0;

    ASTNode *root = create_list_node();

    while (!match(&parser, TOKEN_EOF)) {
        ASTNode *expr = parse_expression(&parser);
        if (!expr) {
            free_ast(root);
            return NULL;
        }
        add_to_list(root, expr);
    }

    if (root->as.list.count == 1) {
        ASTNode *single = root->as.list.elements[0];
        free(root->as.list.elements);
        free(root);
        return single;
    }

    return root;
}
