#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tac.h"

static int temp_counter = 0;
static int label_counter = 0;

static char *dup_string(const char *text) {
    char *copy = malloc(strlen(text) + 1);
    strcpy(copy, text);
    return copy;
}

static int is_kind(ASTNode *node, const char *kind) {
    return node != NULL && strcmp(node->tipo_no, kind) == 0;
}

TACProgram *tac_program_create(void) {
    TACProgram *program = malloc(sizeof(TACProgram));

    if(program == NULL)
        return NULL;

    program->head = NULL;
    program->tail = NULL;
    return program;
}

TACInstr *tac_instr_create(TACKind kind,
                           const char *result,
                           const char *arg1,
                           const char *arg2,
                           const char *op) {
    TACInstr *instr = malloc(sizeof(TACInstr));

    if(instr == NULL)
        return NULL;

    instr->kind = kind;
    instr->result = result == NULL ? NULL : dup_string(result);
    instr->arg1 = arg1 == NULL ? NULL : dup_string(arg1);
    instr->arg2 = arg2 == NULL ? NULL : dup_string(arg2);
    instr->op = op == NULL ? NULL : dup_string(op);
    instr->next = NULL;

    return instr;
}

void tac_program_append(TACProgram *program, TACInstr *instr) {
    if(program == NULL || instr == NULL)
        return;

    if(program->head == NULL) {
        program->head = instr;
        program->tail = instr;
        return;
    }

    program->tail->next = instr;
    program->tail = instr;
}

void tac_program_free(TACProgram *program) {
    if(program == NULL)
        return;

    TACInstr *instr = program->head;

    while(instr != NULL) {
        TACInstr *next = instr->next;
        free(instr->result);
        free(instr->arg1);
        free(instr->arg2);
        free(instr->op);
        free(instr);
        instr = next;
    }

    free(program);
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

static void emit_program(TACProgram *program,
                         TACKind kind,
                         const char *result,
                         const char *arg1,
                         const char *arg2,
                         const char *op) {
    tac_program_append(program, tac_instr_create(kind, result, arg1, arg2, op));
}

static void print_instr(const TACInstr *instr) {
    switch(instr->kind) {
        case TAC_KIND_ASSIGN:
            printf("%s = %s", instr->result, instr->arg1);
            break;
        case TAC_KIND_BINARY:
            printf("%s = %s %s %s", instr->result, instr->arg1, instr->op, instr->arg2);
            break;
        case TAC_KIND_UNARY:
            printf("%s = %s%s", instr->result, instr->op, instr->arg1);
            break;
        case TAC_KIND_PRINT:
            printf("print %s", instr->arg1);
            break;
        case TAC_KIND_IF_FALSE:
            printf("ifFalse %s goto %s", instr->arg1, instr->result);
            break;
        case TAC_KIND_LABEL:
            printf("%s:", instr->result);
            break;
    }
    printf("\n");
}

void tac_print(const TACProgram *program, const char *title) {
    if(title != NULL)
        printf("%s", title);

    if(program == NULL)
        return;

    for(TACInstr *instr = program->head; instr != NULL; instr = instr->next)
        print_instr(instr);
}

static char *gen_expr(ASTNode *node, TACProgram *program);
static void gen_stmt(ASTNode *node, TACProgram *program);

static void gen_list(ASTNode *node, TACProgram *program) {
    if(node == NULL)
        return;

    if(is_kind(node, "lista_comandos")) {
        gen_list(node->esquerda, program);
        gen_list(node->direita, program);
        return;
    }

    gen_stmt(node, program);
}

static char *gen_expr(ASTNode *node, TACProgram *program) {
    if(node == NULL)
        return dup_string("0");

    if(is_kind(node, "numero") || is_kind(node, "string") || is_kind(node, "identificador"))
        return dup_string(node->valor);

    if(is_kind(node, "operacao")) {
        char *left = gen_expr(node->esquerda, program);
        char *right = gen_expr(node->direita, program);
        char *temp = new_temp();

        if(node->direita == NULL || strcmp(node->valor, "!") == 0) {
            emit_program(program, TAC_KIND_UNARY, temp, left, NULL, node->valor);
        } else {
            emit_program(program, TAC_KIND_BINARY, temp, left, right, node->valor);
        }

        free(left);
        free(right);
        return temp;
    }

    return dup_string("0");
}

static void gen_stmt(ASTNode *node, TACProgram *program) {
    if(node == NULL)
        return;

    if(is_kind(node, "lista_comandos")) {
        gen_list(node, program);
        return;
    }

    if(is_kind(node, "declaracao"))
        return;

    if(is_kind(node, "atribuicao")) {
        char *rhs = gen_expr(node->esquerda, program);
        emit_program(program, TAC_KIND_ASSIGN, node->valor, rhs, NULL, NULL);
        free(rhs);
        return;
    }

    if(is_kind(node, "print")) {
        emit_program(program, TAC_KIND_PRINT, NULL, node->valor, NULL, NULL);
        return;
    }

    if(is_kind(node, "if")) {
        char *cond = gen_expr(node->esquerda, program);
        char *label_false = new_label();

        emit_program(program, TAC_KIND_IF_FALSE, label_false, cond, NULL, NULL);
        gen_stmt(node->direita, program);
        emit_program(program, TAC_KIND_LABEL, label_false, NULL, NULL, NULL);

        free(cond);
        free(label_false);
        return;
    }

    gen_list(node, program);
}

TACProgram *tac_build(ASTNode *root) {
    temp_counter = 0;
    label_counter = 0;

    TACProgram *program = tac_program_create();

    if(program == NULL)
        return NULL;

    gen_stmt(root, program);
    return program;
}

void tac_generate(ASTNode *root) {
    TACProgram *program = tac_build(root);

    printf("\nTAC:\n");
    tac_print(program, NULL);
    tac_program_free(program);
}