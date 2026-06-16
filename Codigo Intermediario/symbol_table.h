#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define MAX_SYMBOLS 100

typedef struct {
    char nome[50];
    char tipo[20];
} Simbolo;

extern Simbolo tabela[MAX_SYMBOLS];
extern int total_simbolos;

void inserir_simbolo(char *nome, char *tipo);
int simbolo_existe(char *nome);
char* obter_tipo(char *nome);

#endif