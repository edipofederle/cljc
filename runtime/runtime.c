#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void print_double(double value) {
    printf("Result: %f\n", value);
}

// String runtime functions
double str_length(const char *s) {
    return (double)strlen(s);
}

double str_char_at(const char *s, double index) {
    int idx = (int)index;
    if (idx < 0 || idx >= (int)strlen(s)) {
        return 0.0;
    }
    return (double)s[idx];
}

char* str_concat(const char *s1, const char *s2) {
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    char *result = malloc(len1 + len2 + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char* substring(const char *s, double start, double end) {
    int st = (int)start;
    int en = (int)end;
    int len = (int)strlen(s);

    if (st < 0) st = 0;
    if (en > len) en = len;
    if (st >= en) {
        char *empty = malloc(1);
        empty[0] = '\0';
        return empty;
    }

    int result_len = en - st;
    char *result = malloc(result_len + 1);
    strncpy(result, s + st, result_len);
    result[result_len] = '\0';
    return result;
}

// Simple runtime list structure (for POC - stores doubles)
typedef struct RuntimeList {
    double *elements;
    int count;
    int capacity;
} RuntimeList;

RuntimeList* create_list() {
    RuntimeList *list = malloc(sizeof(RuntimeList));
    list->capacity = 8;
    list->count = 0;
    list->elements = malloc(list->capacity * sizeof(double));
    return list;
}

RuntimeList* cons(double elem, RuntimeList *lst) {
    if (!lst) {
        lst = create_list();
    }

    if (lst->count >= lst->capacity) {
        lst->capacity *= 2;
        lst->elements = realloc(lst->elements, lst->capacity * sizeof(double));
    }

    // Shift all elements right
    for (int i = lst->count; i > 0; i--) {
        lst->elements[i] = lst->elements[i-1];
    }
    lst->elements[0] = elem;
    lst->count++;

    return lst;
}

double first(RuntimeList *lst) {
    if (!lst || lst->count == 0) {
        return 0.0;
    }
    return lst->elements[0];
}

RuntimeList* rest(RuntimeList *lst) {
    if (!lst || lst->count <= 1) {
        return create_list();
    }

    RuntimeList *result = create_list();
    for (int i = 1; i < lst->count; i++) {
        if (result->count >= result->capacity) {
            result->capacity *= 2;
            result->elements = realloc(result->elements, result->capacity * sizeof(double));
        }
        result->elements[result->count++] = lst->elements[i];
    }

    return result;
}

RuntimeList* append_elem(RuntimeList *lst, double elem) {
    if (!lst) {
        lst = create_list();
    }

    if (lst->count >= lst->capacity) {
        lst->capacity *= 2;
        lst->elements = realloc(lst->elements, lst->capacity * sizeof(double));
    }

    lst->elements[lst->count++] = elem;
    return lst;
}

double list_count(RuntimeList *lst) {
    if (!lst) return 0.0;
    return (double)lst->count;
}
