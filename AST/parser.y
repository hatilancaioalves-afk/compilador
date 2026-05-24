%{
#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "symbol_table.h"

int yylex(void);
int yyerror(const char *s);

ASTNode *raiz;
char tipo_atual[20];
%}

%code requires {
#include "ast.h"
}

%union {
    char *str;
    ASTNode *node;
}

%type <node> lista_comandos
%type <node> comando
%type <node> declaracao
%type <node> atribuicao
%type <node> comando_if
%type <node> comando_print
%type <node> expressao

%token <str> IDENTIFICADOR
%token <str> NUMERO
%token <str> STRING_LITERAL

%token INT FLOAT STRING BOOL
%token IF ELSE FOR PRINT BEGINN END

%token OP_REL
%token OP_LOGICO
%token OP_ARIT
%token ATRIB

%token VIRGULA
%token PONTO_VIRGULA
%token ABRE_PAR FECHA_PAR
%token ABRE_CHAVE FECHA_CHAVE

%left OP_LOGICO
%nonassoc OP_REL
%left OP_ARIT

%%

programa:
      BEGINN ABRE_CHAVE lista_comandos FECHA_CHAVE END
      {
          raiz = $3;
          printf("\nAST:\n");
          imprimir_ast(raiz, 0);
      }
    ;

lista_comandos:
      lista_comandos comando
      {
          $$ = criar_no("lista_comandos", "seq", "void", $1, $2);
      }
    | comando
      {
          $$ = $1;
      }
    ;

comando:
      declaracao
    | atribuicao
    | comando_if
    | comando_print
    ;

declaracao:
      INT
      {
          strcpy(tipo_atual, "int");
      }
      lista_identificadores PONTO_VIRGULA
      {
          $$ = criar_no("declaracao", "int", "void", NULL, NULL);
      }
    | FLOAT
      {
          strcpy(tipo_atual, "float");
      }
      lista_identificadores PONTO_VIRGULA
      {
          $$ = criar_no("declaracao", "float", "void", NULL, NULL);
      }
    | STRING
      {
          strcpy(tipo_atual, "string");
      }
      lista_identificadores PONTO_VIRGULA
      {
          $$ = criar_no("declaracao", "string", "void", NULL, NULL);
      }
    | BOOL
      {
          strcpy(tipo_atual, "bool");
      }
      lista_identificadores PONTO_VIRGULA
      {
          $$ = criar_no("declaracao", "bool", "void", NULL, NULL);
      }
    ;

lista_identificadores:
      IDENTIFICADOR
      {
          inserir_simbolo($1, tipo_atual);
          printf("Variável '%s' declarada como %s\n", $1, tipo_atual);
      }
    | lista_identificadores VIRGULA IDENTIFICADOR
      {
          inserir_simbolo($3, tipo_atual);
          printf("Variável '%s' declarada como %s\n", $3, tipo_atual);
      }
    ;

atribuicao:
      IDENTIFICADOR ATRIB expressao PONTO_VIRGULA
      {
          if(!simbolo_existe($1)) {
              printf("Erro semântico: variável '%s' não declarada\n", $1);
          } else if($3 != NULL && strcmp(obter_tipo($1), $3->tipo) != 0) {
              printf("Erro semântico: tipos incompatíveis em atribuição\n");
          }

          $$ = criar_no("atribuicao", $1, "void", $3, NULL);
      }
    ;

comando_if:
      IF ABRE_PAR expressao FECHA_PAR ABRE_CHAVE lista_comandos FECHA_CHAVE
      {
          $$ = criar_no("if", "if", "void", $3, $6);
      }
    ;

comando_print:
      PRINT IDENTIFICADOR PONTO_VIRGULA
      {
          if(!simbolo_existe($2)) {
              printf("Erro semântico: variável '%s' não declarada\n", $2);
          }

          $$ = criar_no("print", $2, "void", NULL, NULL);
      }
    ;

expressao:
      NUMERO
      {
          if(strchr($1, '.') != NULL)
              $$ = criar_no("numero", $1, "float", NULL, NULL);
          else
              $$ = criar_no("numero", $1, "int", NULL, NULL);
      }
    | STRING_LITERAL
      {
          $$ = criar_no("string", $1, "string", NULL, NULL);
      }
    | IDENTIFICADOR
      {
          if(!simbolo_existe($1)) {
              printf("Erro semântico: variável '%s' não declarada\n", $1);
              $$ = NULL;
          } else {
              $$ = criar_no("identificador", $1, obter_tipo($1), NULL, NULL);
          }
      }
    | expressao OP_ARIT expressao
      {
          if($1 == NULL || $3 == NULL) {
              $$ = NULL;
          } else if(strcmp($1->tipo, "string") == 0 || strcmp($3->tipo, "string") == 0) {
              printf("Erro semântico: operação aritmética inválida com string\n");
              $$ = criar_no("erro", "aritmetica", "erro", $1, $3);
          } else if(strcmp($1->tipo, "float") == 0 || strcmp($3->tipo, "float") == 0) {
              $$ = criar_no("operacao", "arit", "float", $1, $3);
          } else {
              $$ = criar_no("operacao", "arit", "int", $1, $3);
          }
      }
    | expressao OP_REL expressao
      {
          $$ = criar_no("operacao", "rel", "bool", $1, $3);
      }
    | expressao OP_LOGICO expressao
      {
          $$ = criar_no("operacao", "logico", "bool", $1, $3);
      }
    | ABRE_PAR expressao FECHA_PAR
      {
          $$ = $2;
      }
    ;

%%

int main(void) {
    printf("Iniciando analise sintatica...\n");

    if(yyparse() == 0) {
        printf("Analise sintatica concluida com sucesso.\n");
        return 0;
    }

    printf("Analise sintatica concluida com erro.\n");
    return 1;
}

int yyerror(const char *s) {
    printf("Erro sintatico: %s\n", s);
    return 0;
}
