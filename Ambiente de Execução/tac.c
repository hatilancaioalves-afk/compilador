#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "tac.h"

static int temp_counter = 0;
static int label_counter = 0;

static char *dup_string(const char *text) {
    char *copy = malloc(strlen(text) + 1);
    strcpy(copy, text);
    return copy;
}

static char *new_temp(void) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "t%d", ++temp_counter);
    return dup_string(buffer);
}

static char *new_label(void) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "L%d", ++label_counter);
    return dup_string(buffer);
}

static void emit(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

static int is_kind(ASTNode *node, const char *kind) {
    return node != NULL && strcmp(node->tipo_no, kind) == 0;
}

static char *gen_expr(ASTNode *node);
static void gen_stmt(ASTNode *node);

static void gen_list(ASTNode *node) {
    if(node == NULL)
        return;

    if(is_kind(node, "lista_comandos")) {
        gen_list(node->esquerda);
        gen_list(node->direita);
        return;
    }

    gen_stmt(node);
}

static char *gen_expr(ASTNode *node) {
    if(node == NULL)
        return dup_string("0");

    if(is_kind(node, "numero") || is_kind(node, "string") || is_kind(node, "identificador"))
        return dup_string(node->valor);

    if(is_kind(node, "chamada_funcao")) {
            char *temp = new_temp();
            emit("%s = call %s", temp, node->valor);
            return temp;
        }    

    if(is_kind(node, "operacao")) {
        char *left = gen_expr(node->esquerda);
        char *right = gen_expr(node->direita);
        char *temp = new_temp();

        if(node->direita == NULL || strcmp(node->valor, "!") == 0) {
            emit("%s = %s%s", temp, node->valor, left);
        } else {
            emit("%s = %s %s %s", temp, left, node->valor, right);
        }

        free(left);
        free(right);
        return temp;
    }

    return dup_string("0");
}

static void gen_stmt(ASTNode *node) {
    if(node == NULL)
        return;

    if(is_kind(node, "lista_comandos")) {
        gen_list(node);
        return;
    }

    if(is_kind(node, "declaracao"))
        return;

    if(is_kind(node, "atribuicao")) {
        char *rhs = gen_expr(node->esquerda);
        emit("%s = %s", node->valor, rhs);
        free(rhs);
        return;
    }

    if(is_kind(node, "print")) {
        emit("print %s", node->valor);
        return;
    }

    if(is_kind(node, "if")) {
        char *cond = gen_expr(node->esquerda);
        char *label_false = new_label();

        emit("ifFalse %s goto %s", cond, label_false);
        gen_stmt(node->direita);
        emit("%s:", label_false);

        free(cond);
        free(label_false);
        return;
    }

    if(is_kind(node, "programa") || is_kind(node, "lista_funcoes")) {
        gen_stmt(node->esquerda);
        gen_stmt(node->direita);
        return;
    }

    if(is_kind(node, "main")) {
        emit("main:");
        gen_stmt(node->direita);
        return;
    }

    if(is_kind(node, "funcao")) {
        emit("%s:", node->valor);
        gen_stmt(node->direita);
        return;
    }

    if(is_kind(node, "return")) {
        char *ret_val = gen_expr(node->esquerda);
        emit("return %s", ret_val);
        free(ret_val);
        return;
    }

    gen_list(node);
}

void tac_generate(ASTNode *root) {
    temp_counter = 0;
    label_counter = 0;

    printf("\nTAC:\n");
    gen_stmt(root);
}