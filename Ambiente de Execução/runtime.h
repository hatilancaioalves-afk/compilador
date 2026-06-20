#ifndef RUNTIME_H
#define RUNTIME_H

#include "ast.h"

typedef struct VariavelSimulada {
    char nome[32];
    int valor;
    struct VariavelSimulada *proximo;
} VariavelSimulada;

typedef struct ActivationRecord {
    char nome_funcao[32];
    
    VariavelSimulada *parametros_formais;
    VariavelSimulada *variaveis_locais;     
    int valor_retorno;                      
    
    struct ActivationRecord *link_dinamico;
    struct ActivationRecord *link_estatico;
    int endereco_retorno;                   
    
    struct ActivationRecord *abaixo;
} ActivationRecord;

void pilha_push(char *nome_funcao, int end_retorno);
void pilha_pop();

#endif