#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

ASTNode* criar_no(char *tipo_no,
                  char *valor,
                  char *tipo,
                  ASTNode *esquerda,
                  ASTNode *direita) {

    ASTNode *novo = malloc(sizeof(ASTNode));

    strcpy(novo->tipo_no, tipo_no);
    strcpy(novo->valor, valor);
    strcpy(novo->tipo, tipo);

    novo->esquerda = esquerda;
    novo->direita = direita;

    return novo;
}

static void imprimir_ast_rec(ASTNode *raiz, const char *prefixo, int ultimo, int eh_raiz) {

    if(raiz == NULL)
        return;

    if(!eh_raiz) {
        printf("%s", prefixo);
        printf("%s", ultimo ? "`-- " : "|-- ");
    }

    printf("%s: %s [%s]\n",
           raiz->tipo_no,
           raiz->valor,
           raiz->tipo);

    char novo_prefixo[256];

    snprintf(novo_prefixo, sizeof(novo_prefixo), "%s%s",
             prefixo,
             eh_raiz ? "" : (ultimo ? "    " : "|   "));

    if(raiz->esquerda != NULL || raiz->direita != NULL) {
        imprimir_ast_rec(raiz->esquerda, novo_prefixo, raiz->direita == NULL, 0);
        imprimir_ast_rec(raiz->direita, novo_prefixo, 1, 0);
    }
}

void imprimir_ast(ASTNode *raiz, int nivel) {
    (void)nivel;
    imprimir_ast_rec(raiz, "", 1, 1);
}