#include <stdio.h>
#include <string.h>
#include "symbol_table.h"

#define MAX_SYMBOLS 100

Simbolo tabela[MAX_SYMBOLS];
int total_simbolos = 0;

void inserir_simbolo(char *nome, char *tipo) {

    if(simbolo_existe(nome)) {
        printf("Erro semântico: '%s' já declarado\n", nome);
        return;
    }

    strcpy(tabela[total_simbolos].nome, nome);
    strcpy(tabela[total_simbolos].tipo, tipo);

    total_simbolos++;
}

int simbolo_existe(char *nome) {

    for(int i = 0; i < total_simbolos; i++) {

        if(strcmp(tabela[i].nome, nome) == 0)
            return 1;
    }

    return 0;
}

char* obter_tipo(char *nome) {

    for(int i = 0; i < total_simbolos; i++) {

        if(strcmp(tabela[i].nome, nome) == 0)
            return tabela[i].tipo;
    }

    return NULL;
}