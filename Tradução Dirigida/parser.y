%{
#include <stdio.h>
#include <string.h>
#include "symbol_table.h"

int yylex();
int yyerror(const char *s);

char tipo_atual[20];
int erro_semantico = 0;
%}

/* =========================
   UNION
========================= */

%union {
    char *str;
    char *tipo;
}

/* =========================
   TYPES
========================= */

%type <tipo> expressao

/* =========================
   TOKENS
========================= */

%token <str> IDENTIFICADOR
%token <str> NUMERO
%token <str> STRING_LITERAL

/* Palavras-chave */

%token INT FLOAT STRING BOOL
%token IF ELSE FOR PRINT BEGINN END

/* Operadores */

%token OP_REL
%token OP_LOGICO
%token OP_ARIT
%token ATRIB

/* Delimitadores */

%token VIRGULA
%token PONTO_VIRGULA
%token ABRE_PAR FECHA_PAR
%token ABRE_CHAVE FECHA_CHAVE

/* Precedência */

%left OP_LOGICO
%nonassoc OP_REL
%left OP_ARIT

%%

/* =========================
   PROGRAMA
========================= */

programa:
      lista_comandos
    ;

/* =========================
   LISTA DE COMANDOS
========================= */

lista_comandos:
      lista_comandos comando
    | comando
    ;

/* =========================
   COMANDOS
========================= */

comando:
      declaracao
    | atribuicao
    | comando_if
    | comando_print
    ;

/* =========================
   DECLARAÇÃO
========================= */

declaracao:
      tipo lista_identificadores PONTO_VIRGULA
    ;

/* =========================
   LISTA DE IDENTIFICADORES
========================= */

lista_identificadores:

      IDENTIFICADOR
      {
          inserir_simbolo($1, tipo_atual);

          printf("Variável '%s' declarada como %s\n",
                 $1,
                 tipo_atual);
      }

    | lista_identificadores VIRGULA IDENTIFICADOR
      {
          inserir_simbolo($3, tipo_atual);

          printf("Variável '%s' declarada como %s\n",
                 $3,
                 tipo_atual);
      }
    ;

/* =========================
   TIPOS
========================= */

tipo:

      INT
      {
          strcpy(tipo_atual, "int");
      }

    | FLOAT
      {
          strcpy(tipo_atual, "float");
      }

    | STRING
      {
          strcpy(tipo_atual, "string");
      }

    | BOOL
      {
          strcpy(tipo_atual, "bool");
      }
    ;

/* =========================
   ATRIBUIÇÃO
========================= */

atribuicao:

      IDENTIFICADOR ATRIB expressao PONTO_VIRGULA
      {
          if(!simbolo_existe($1)) {
              erro_semantico = 1;

              printf("Erro semântico: variável '%s' não declarada\n",
                     $1);
          }
          else {

              char *tipo_var = obter_tipo($1);

              if(strcmp(tipo_var, $3) != 0) {
                  erro_semantico = 1;

                  printf("Erro semântico: tipos incompatíveis em atribuição\n");
              }
          }
      }
    ;

/* =========================
   IF
========================= */

comando_if:

      IF ABRE_PAR expressao FECHA_PAR
      ABRE_CHAVE lista_comandos FECHA_CHAVE
    ;

/* =========================
   PRINT
========================= */

comando_print:

      PRINT IDENTIFICADOR PONTO_VIRGULA
      {
          if(!simbolo_existe($2)) {
              erro_semantico = 1;

              printf("Erro semântico: variável '%s' não declarada\n",
                     $2);
          }
      }
    ;

/* =========================
   EXPRESSÕES
========================= */

expressao:

      NUMERO
      {
          if(strchr($1, '.'))
              $$ = "float";
          else
              $$ = "int";
      }

    | STRING_LITERAL
      {
          $$ = "string";
      }

    | IDENTIFICADOR
      {
          if(!simbolo_existe($1)) {
              erro_semantico = 1;

              printf("Erro semântico: variável '%s' não declarada\n",
                     $1);

              $$ = "erro";
          }
          else {

              $$ = obter_tipo($1);
          }
      }

    | expressao OP_ARIT expressao
      {
          if(strcmp($1, "string") == 0 ||
             strcmp($3, "string") == 0) {
              erro_semantico = 1;

              printf("Erro semântico: operação aritmética inválida com string\n");

              $$ = "erro";
          }
          else {

              if(strcmp($1, "float") == 0 ||
                 strcmp($3, "float") == 0)
                  $$ = "float";
              else
                  $$ = "int";
          }
      }

    | expressao OP_REL expressao
      {
          if(strcmp($1, $3) != 0) {
              erro_semantico = 1;

              printf("Erro semântico: comparação entre tipos incompatíveis\n");

              $$ = "erro";
          }
          else {

              $$ = "bool";
          }
      }

    | expressao OP_LOGICO expressao
      {
          $$ = "bool";
      }

    | ABRE_PAR expressao FECHA_PAR
      {
          $$ = $2;
      }
    ;

%%

/* =========================
   MAIN
========================= */

int main() {

    printf("Iniciando analise sintatica...\n");

    if(yyparse() == 0 && erro_semantico == 0) {
        printf("Analise sintatica concluida com sucesso.\n");
        return 0;
    }

    printf("Analise sintatica concluida com erro.\n");
    return 1;
}

/* =========================
   ERROS
========================= */

int yyerror(const char *s) {

    printf("Erro sintatico: %s\n", s);

    return 0;
}