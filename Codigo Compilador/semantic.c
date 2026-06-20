#include <stdio.h>
#include <string.h>
#include "semantic.h"
#include "symbol_table.h"

static char current_type[20];
static int semantic_error = 0;

void semantic_init(void) {
    current_type[0] = '\0';
    semantic_error = 0;
}

void semantic_set_current_type(const char *type_name) {
    strncpy(current_type, type_name, sizeof(current_type) - 1);
    current_type[sizeof(current_type) - 1] = '\0';
}

void semantic_declare_identifier(char *name) {
    if(simbolo_existe(name)) {
        semantic_error = 1;
        printf("Erro semântico: '%s' já declarado\n", name);
        return;
    }

    inserir_simbolo(name, current_type);
    printf("Variável '%s' declarada como %s\n", name, current_type);
}

void semantic_check_assignment(char *name, const char *expr_type) {
    if(!simbolo_existe(name)) {
        semantic_error = 1;
        printf("Erro semântico: variável '%s' não declarada\n", name);
        return;
    }

    const char *tipo_var = obter_tipo(name);

    if(expr_type != NULL && strcmp(tipo_var, expr_type) != 0) {
        semantic_error = 1;
        printf("Erro semântico: tipos incompatíveis em atribuição\n");
    }
}

void semantic_check_print(char *name) {
    if(!simbolo_existe(name)) {
        semantic_error = 1;
        printf("Erro semântico: variável '%s' não declarada\n", name);
    }
}

const char *semantic_type_of_identifier(char *name) {
    if(!simbolo_existe(name)) {
        semantic_error = 1;
        printf("Erro semântico: variável '%s' não declarada\n", name);
        return "erro";
    }

    return obter_tipo(name);
}

const char *semantic_check_arithmetic(const char *left_type, const char *right_type) {
    if(left_type == NULL || right_type == NULL)
        return "erro";

    if(strcmp(left_type, "string") == 0 || strcmp(right_type, "string") == 0) {
        semantic_error = 1;
        printf("Erro semântico: operação aritmética inválida com string\n");
        return "erro";
    }

    if(strcmp(left_type, "float") == 0 || strcmp(right_type, "float") == 0)
        return "float";

    return "int";
}

const char *semantic_check_relational(const char *left_type, const char *right_type) {
    if(left_type == NULL || right_type == NULL)
        return "erro";

    if(strcmp(left_type, right_type) != 0) {
        semantic_error = 1;
        printf("Erro semântico: comparação entre tipos incompatíveis\n");
        return "erro";
    }

    return "bool";
}

const char *semantic_check_logical(const char *left_type, const char *right_type) {
    if(left_type == NULL || right_type == NULL)
        return "erro";

    return "bool";
}

int semantic_has_error(void) {
    return semantic_error;
}