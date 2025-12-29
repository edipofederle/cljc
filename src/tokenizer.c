#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokenizer.h"

#define INITIAL_CAPACITY 16

typedef struct {
    const char *source;
    int current;
    int line;
    int column;
} Tokenizer;

static void init_tokenizer(Tokenizer *t, const char *source) {
    t->source = source;
    t->current = 0;
    t->line = 1;
    t->column = 1;
}

static char peek(Tokenizer *t) {
    return t->source[t->current];
}

static char advance(Tokenizer *t) {
    char c = t->source[t->current++];
    if (c == '\n') {
        t->line++;
        t->column = 1;
    } else {
        t->column++;
    }
    return c;
}

static int is_at_end(Tokenizer *t) {
    return t->source[t->current] == '\0';
}

static void skip_whitespace(Tokenizer *t) {
    while (!is_at_end(t)) {
        char c = peek(t);
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ',') {
            advance(t);
        } else {
            break;
        }
    }
}

static Token create_token(TokenType type, const char *value, int line, int column) {
    Token token;
    token.type = type;
    token.value = value ? strdup(value) : NULL;
    token.line = line;
    token.column = column;
    return token;
}

static void add_token(TokenList *list, Token token) {
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->tokens = realloc(list->tokens, list->capacity * sizeof(Token));
    }
    list->tokens[list->count++] = token;
}

static int is_symbol_char(char c) {
    return isalnum(c) || c == '+' || c == '-' || c == '*' || c == '/' ||
           c == '=' || c == '!' || c == '<' || c == '>' || c == '?' ||
           c == '_' || c == '.';
}

static Token tokenize_number(Tokenizer *t) {
    int start = t->current;
    int start_col = t->column;

    while (!is_at_end(t) && (isdigit(peek(t)) || peek(t) == '.')) {
        advance(t);
    }

    int length = t->current - start;
    char *value = malloc(length + 1);
    strncpy(value, t->source + start, length);
    value[length] = '\0';

    Token token = create_token(TOKEN_NUMBER, value, t->line, start_col);
    free(value);
    return token;
}

static Token tokenize_symbol(Tokenizer *t) {
    int start = t->current;
    int start_col = t->column;

    while (!is_at_end(t) && is_symbol_char(peek(t))) {
        advance(t);
    }

    int length = t->current - start;
    char *value = malloc(length + 1);
    strncpy(value, t->source + start, length);
    value[length] = '\0';

    Token token = create_token(TOKEN_SYMBOL, value, t->line, start_col);
    free(value);
    return token;
}

static Token tokenize_string(Tokenizer *t) {
    int start_line = t->line;
    int start_col = t->column;

    // Skip opening quote
    advance(t);
    int start = t->current;

    // Read until closing quote or end of input
    while (!is_at_end(t) && peek(t) != '"') {
        if (peek(t) == '\\') {
            // Skip escape sequence
            advance(t);
            if (!is_at_end(t)) {
                advance(t);
            }
        } else {
            advance(t);
        }
    }

    if (is_at_end(t)) {
        return create_token(TOKEN_ERROR, "Unterminated string", start_line, start_col);
    }

    int length = t->current - start;
    char *value = malloc(length + 1);
    strncpy(value, t->source + start, length);
    value[length] = '\0';

    // Skip closing quote
    advance(t);

    Token token = create_token(TOKEN_STRING, value, start_line, start_col);
    free(value);
    return token;
}

TokenList *tokenize(const char *source) {
    Tokenizer t;
    init_tokenizer(&t, source);

    TokenList *list = malloc(sizeof(TokenList));
    list->capacity = INITIAL_CAPACITY;
    list->count = 0;
    list->tokens = malloc(list->capacity * sizeof(Token));

    while (!is_at_end(&t)) {
        skip_whitespace(&t);

        if (is_at_end(&t)) {
            break;
        }

        char c = peek(&t);
        int col = t.column;

        if (c == '(') {
            advance(&t);
            add_token(list, create_token(TOKEN_LEFT_PAREN, "(", t.line, col));
        } else if (c == ')') {
            advance(&t);
            add_token(list, create_token(TOKEN_RIGHT_PAREN, ")", t.line, col));
        } else if (c == '[') {
            advance(&t);
            add_token(list, create_token(TOKEN_LEFT_BRACKET, "[", t.line, col));
        } else if (c == ']') {
            advance(&t);
            add_token(list, create_token(TOKEN_RIGHT_BRACKET, "]", t.line, col));
        } else if (c == '"') {
            add_token(list, tokenize_string(&t));
        } else if (isdigit(c)) {
            add_token(list, tokenize_number(&t));
        } else if (is_symbol_char(c)) {
            add_token(list, tokenize_symbol(&t));
        } else {
            char error_msg[64];
            snprintf(error_msg, sizeof(error_msg), "Unexpected character: '%c'", c);
            add_token(list, create_token(TOKEN_ERROR, error_msg, t.line, col));
            advance(&t);
        }
    }

    add_token(list, create_token(TOKEN_EOF, NULL, t.line, t.column));
    return list;
}

void free_tokens(TokenList *list) {
    for (int i = 0; i < list->count; i++) {
        free(list->tokens[i].value);
    }
    free(list->tokens);
    free(list);
}

const char *token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_LEFT_PAREN: return "LEFT_PAREN";
        case TOKEN_RIGHT_PAREN: return "RIGHT_PAREN";
        case TOKEN_LEFT_BRACKET: return "LEFT_BRACKET";
        case TOKEN_RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_SYMBOL: return "SYMBOL";
        case TOKEN_STRING: return "STRING";
        case TOKEN_EOF: return "EOF";
        case TOKEN_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

void print_tokens(TokenList *list) {
    printf("Tokens (%d):\n", list->count);
    for (int i = 0; i < list->count; i++) {
        Token *t = &list->tokens[i];
        printf("  [%d:%d] %-15s", t->line, t->column, token_type_to_string(t->type));
        if (t->value) {
            printf(" '%s'", t->value);
        }
        printf("\n");
    }
}
