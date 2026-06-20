#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "runtime.h"

static ActivationRecord *topo_pilha = NULL;

void pilha_push(char *nome_funcao, int end_retorno) {
    ActivationRecord *novo_ar = malloc(sizeof(ActivationRecord));
    strcpy(novo_ar->nome_funcao, nome_funcao);
    novo_ar->endereco_retorno = end_retorno;
    novo_ar->valor_retorno = 0;
    novo_ar->parametros_formais = NULL;
    novo_ar->variaveis_locais = NULL;
    
    novo_ar->link_dinamico = topo_pilha;
    novo_ar->link_estatico = NULL; 
    
    novo_ar->abaixo = topo_pilha;
    topo_pilha = novo_ar;
    
    printf("[call] %s (Registro de ativação criado)\n", nome_funcao);
}

void pilha_pop() {
    if (topo_pilha == NULL) return;
    
    ActivationRecord *antigo_topo = topo_pilha;
    
    printf("[return] %s = %d (Registro de ativação removido)\n", antigo_topo->nome_funcao, antigo_topo->valor_retorno);
    
    topo_pilha = topo_pilha->abaixo;
    
    free(antigo_topo);
}