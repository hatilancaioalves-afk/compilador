#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

typedef struct {
    char nome[50];
    char tipo[20];
} Simbolo;

void inserir_simbolo(char *nome, char *tipo);
int simbolo_existe(char *nome);
char* obter_tipo(char *nome);

#endif