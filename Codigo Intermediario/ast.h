#ifndef AST_H
#define AST_H

typedef struct ASTNode {

    char tipo_no[20];
    char valor[50];

    char tipo[20];

    struct ASTNode *esquerda;
    struct ASTNode *direita;

} ASTNode;

ASTNode* criar_no(char *tipo_no,
                  char *valor,
                  char *tipo,
                  ASTNode *esquerda,
                  ASTNode *direita);

void imprimir_ast(ASTNode *raiz, int nivel);

#endif