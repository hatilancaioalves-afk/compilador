%{
#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "semantic.h"
#include "tac.h"
#include "optimizer.h"
#include "codegen_c.h"
#include "codegen_python.h"

int yylex(void);
int yyerror(const char *s);

ASTNode *raiz;
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

%token <str> OP_REL
%token <str> OP_LOGICO
%token <str> OP_ADD
%token <str> OP_MUL
%token ATRIB

%token VIRGULA
%token PONTO_VIRGULA
%token ABRE_PAR FECHA_PAR
%token ABRE_CHAVE FECHA_CHAVE

%left OP_LOGICO
%nonassoc OP_REL
%left OP_ADD
%left OP_MUL

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
                    semantic_set_current_type("int");
      }
      lista_identificadores PONTO_VIRGULA
      {
          $$ = criar_no("declaracao", "int", "void", NULL, NULL);
      }
    | FLOAT
      {
                    semantic_set_current_type("float");
      }
      lista_identificadores PONTO_VIRGULA
      {
          $$ = criar_no("declaracao", "float", "void", NULL, NULL);
      }
    | STRING
      {
                    semantic_set_current_type("string");
      }
      lista_identificadores PONTO_VIRGULA
      {
          $$ = criar_no("declaracao", "string", "void", NULL, NULL);
      }
    | BOOL
      {
                    semantic_set_current_type("bool");
      }
      lista_identificadores PONTO_VIRGULA
      {
          $$ = criar_no("declaracao", "bool", "void", NULL, NULL);
      }
    ;

lista_identificadores:
      IDENTIFICADOR
      {
                    semantic_declare_identifier($1);
      }
    | lista_identificadores VIRGULA IDENTIFICADOR
      {
                    semantic_declare_identifier($3);
      }
    ;

atribuicao:
      IDENTIFICADOR ATRIB expressao PONTO_VIRGULA
      {
          semantic_check_assignment($1, $3 == NULL ? NULL : $3->tipo);

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
          semantic_check_print($2);

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
          const char *tipo = semantic_type_of_identifier($1);

          if(strcmp(tipo, "erro") == 0) {
              $$ = NULL;
          } else {
              $$ = criar_no("identificador", $1, (char *)tipo, NULL, NULL);
          }
      }
    | expressao OP_ADD expressao
      {
          const char *tipo = semantic_check_arithmetic($1 == NULL ? NULL : $1->tipo,
                                                       $3 == NULL ? NULL : $3->tipo);

          $$ = criar_no("operacao", $2, (char *)tipo, $1, $3);
      }
    | expressao OP_MUL expressao
      {
          const char *tipo = semantic_check_arithmetic($1 == NULL ? NULL : $1->tipo,
                                                       $3 == NULL ? NULL : $3->tipo);

          $$ = criar_no("operacao", $2, (char *)tipo, $1, $3);
      }
    | expressao OP_REL expressao
      {
          const char *tipo = semantic_check_relational($1 == NULL ? NULL : $1->tipo,
                                                        $3 == NULL ? NULL : $3->tipo);

          $$ = criar_no("operacao", $2, (char *)tipo, $1, $3);
      }
    | expressao OP_LOGICO expressao
      {
          const char *tipo = semantic_check_logical($1 == NULL ? NULL : $1->tipo,
                                                    $3 == NULL ? NULL : $3->tipo);

          $$ = criar_no("operacao", $2, (char *)tipo, $1, $3);
      }
    | ABRE_PAR expressao FECHA_PAR
      {
          $$ = $2;
      }
    ;

%%

int main(void) {
    printf("Iniciando analise sintatica...\n");

  semantic_init();

  if(yyparse() == 0 && !semantic_has_error()) {
      TACProgram *raw = tac_build(raiz);
      TACProgram *optimized = tac_optimize(raw);

      printf("\nTAC:\n");
      tac_print(raw, NULL);
      printf("\nTAC otimizado:\n");
      tac_print(optimized, NULL);
      codegen_python_generate(raiz);
      codegen_c_generate_from_tac(optimized);

      tac_program_free(raw);
      tac_program_free(optimized);
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
