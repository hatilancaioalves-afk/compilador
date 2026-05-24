%{
#include <stdio.h>

int yylex();
int yyerror(const char *s);

%}

/* Palavras-chave */
%token INT FLOAT STRING BOOL
%token IF ELSE FOR PRINT BEGINN END

/* Tokens gerais */
%token IDENTIFICADOR NUMERO

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

%left OP_LOGICO
%nonassoc OP_REL
%left OP_ARIT

%%

programa:
      lista_comandos
    ;

lista_comandos:
      lista_comandos comando
    | comando
    ;

comando:
      declaracao
    | atribuicao
    | comando_if
    | comando_print
    ;

declaracao:
      tipo lista_identificadores PONTO_VIRGULA
    ;

lista_identificadores:
      IDENTIFICADOR
    | lista_identificadores VIRGULA IDENTIFICADOR
    ;

tipo:
      INT
    | FLOAT
    | STRING
    | BOOL
    ;

atribuicao:
      IDENTIFICADOR ATRIB expressao PONTO_VIRGULA
    ;

comando_if:
      IF ABRE_PAR expressao FECHA_PAR
      ABRE_CHAVE lista_comandos FECHA_CHAVE
    ;

comando_print:
      PRINT IDENTIFICADOR PONTO_VIRGULA
    ;

expressao:
      NUMERO
    | IDENTIFICADOR
    | expressao OP_ARIT expressao
    | expressao OP_REL expressao
    | expressao OP_LOGICO expressao
    | ABRE_PAR expressao FECHA_PAR
    ;

%%

int main() {
    printf("Iniciando analise sintatica...\n");

    yyparse();

    printf("Analise sintatica concluida com sucesso.\n");

    return 0;
}

int yyerror(const char *s) {
    printf("Erro sintatico: %s\n", s);
    return 0;
}