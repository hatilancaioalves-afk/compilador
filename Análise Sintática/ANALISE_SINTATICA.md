# Análise Sintática — Explicação detalhada

Este documento descreve a implementação da etapa de análise sintática contida na pasta **Análise Sintática** do projeto. Ele explica os tokens, a gramática definida em `parser.y`, a interação com o lexer (`lexer.l`), e instruções para construir e testar o parser.

## Arquivos principais
- `Análise Sintática/parser.y` — arquivo do Bison que define tokens, precedências, regras da gramática e ações mínimas (entrada e tratamento de erros).
- `Análise Sintática/lexer.l` — arquivo do Flex que reconhece tokens e os retorna ao parser (usa `parser.tab.h`).

## Objetivo
O parser implementa uma gramática simples para uma linguagem imperativa pequena com:
- declaração de variáveis (`int`, `float`, `string`, `bool`)
- atribuição
- condicionais `if` com bloco entre chaves
- impressão `print`
- expressões aritméticas, relacionais e lógicas

O objetivo é reconhecer a estrutura sintática do programa e detectar erros sintáticos.

## Tokens (conforme `lexer.l` e `parser.y`)
O lexer reconhece e retorna os seguintes tokens ao parser:

- Palavras-chave: `INT`, `FLOAT`, `STRING`, `BOOL`, `IF`, `ELSE`, `FOR`, `PRINT`, `BEGINN`, `END`
- Identificador: `IDENTIFICADOR` — reconhecido por `{LETRA}({LETRA}|{DIGITO})*`
- Número: `NUMERO` — reconhecido por `{DIGITO}+(\.{DIGITO}+)?`
- Operadores relacionais: `OP_REL` — `==`, `!=`, `>=`, `<=`, `>`, `<`
- Operadores lógicos: `OP_LOGICO` — `&&`, `||`, `!`
- Operadores aritméticos: `OP_ARIT` — `+`, `-`, `*`, `/`
- Atribuição: `ATRIB` — `=`
- Delimitadores: `VIRGULA` (`,`), `PONTO_VIRGULA` (`;`), `ABRE_PAR` (`(`), `FECHA_PAR` (`)`), `ABRE_CHAVE` (`{`), `FECHA_CHAVE` (`}`)

Observação: o lexer ligado ao parser faz `return TOKEN;` sem atribuir `yylval` para valores — atualmente tokens como `IDENTIFICADOR` e `NUMERO` não propagam um valor semântico (`yylval`) no código desta pasta. Se for necessário usar valores (nomes de identificadores, números), deve-se preencher `yylval` no lexer e ajustar as declarações `%union`/tipos no `parser.y`.

## Gramática (resumo de `parser.y`)

Símbolo inicial: `programa`

Regras principais (forma legível):

- programa -> lista_comandos
- lista_comandos -> lista_comandos comando | comando
- comando -> declaracao | atribuicao | comando_if | comando_print
- declaracao -> tipo lista_identificadores PONTO_VIRGULA
- lista_identificadores -> IDENTIFICADOR | lista_identificadores VIRGULA IDENTIFICADOR
- tipo -> INT | FLOAT | STRING | BOOL
- atribuicao -> IDENTIFICADOR ATRIB expressao PONTO_VIRGULA
- comando_if -> IF ABRE_PAR expressao FECHA_PAR ABRE_CHAVE lista_comandos FECHA_CHAVE
- comando_print -> PRINT IDENTIFICADOR PONTO_VIRGULA
- expressao -> NUMERO | IDENTIFICADOR | expressao OP_ARIT expressao | expressao OP_REL expressao | expressao OP_LOGICO expressao | ABRE_PAR expressao FECHA_PAR

Essa gramática admite blocos de comandos, declarações com listas de identificadores, atribuições terminadas por ponto-e-vírgula, `if` com condição entre parênteses e bloco entre chaves, e `print` que recebe um identificador.

## Precedências e associatividade
As diretivas em `parser.y` definem precedências e associatividade para operadores:

%left OP_LOGICO
%nonassoc OP_REL
%left OP_ARIT

Isto faz com que operadores aritméticos tenham associatividade à esquerda e menor precedência que parênteses; operadores relacionais são não-associativos (evita `a < b < c`), e operadores lógicos têm associatividade à esquerda.

## Ações semânticas e integração com o lexer
- O `parser.y` declara `int yylex();` e `int yyerror(const char*);` e usa `yyparse()` no `main()`.
- O `lexer.l` inclui `parser.tab.h` e retorna os tokens esperados (`return INT;`, etc.).
- Atualmente não há `%union` ou tipos de valores em `parser.y`, então não há passagem formal de valores semânticos via `yylval` entre lexer e parser. Se quiser operar semanticamente (ex.: construir AST, checar tipos, atribuir valores), é necessário:
  - adicionar uma `%union` em `parser.y` com campos para `int`, `double`, `char*`, etc.;
  - declarar os tipos dos símbolos com `%type` e especificar tipos para tokens com `%token <campo>`;
  - no `lexer.l`, atribuir `yylval.campo = ...` antes de `return` para tokens que carregam valor (`NUMERO`, `IDENTIFICADOR`, `STRING_LITERAL`, ...).

## Comandos de build e execução
Exemplo de sequência para gerar o parser e o lexer (executar na pasta `Análise Sintática`):

```bash
bison -d parser.y    # gera parser.tab.c e parser.tab.h
flex lexer.l         # gera lex.yy.c
gcc -o parser parser.tab.c lex.yy.c -lfl
./parser < arquivo_de_entrada.txt
```

Notas:
- Use `-d` no `bison` para gerar o header `parser.tab.h` que o lexer inclui.
- A opção `-lfl` liga a biblioteca flex em sistemas que a exigem.

## Observações e recomendações
- Há diferenças entre os lexers nas pastas do repositório: alguns apenas imprimem tokens (útil para testes e demonstração), enquanto o lexer desta pasta retorna tokens para o parser. Para integração correta, mantenha `lexer.l` retornando os mesmos nomes de token definidos em `parser.y`.
- Se pretende construir árvore sintática (AST) ou checagem semântica, comece adicionando uma `%union` e ações que constroem nós da AST nas regras de `parser.y`.
- Atenção aos literais de string: se quiser suportá-los no parser, o lexer deve retornar um token (ex.: `STRING_LITERAL`) e preencher `yylval.str` com `strdup(yytext)`.

## Exemplo mínimo de programa válido

```
int x, y;
x = 5;
if (x > 3) {
  print x;
}
```

## Próximos passos sugeridos
- Adicionar `%union` e tipos para passar valores semânticos.
- Implementar ações que construam a AST e adicionar um arquivo `README.md` com instruções de uso.

---

Arquivo gerado automaticamente: explicação da pasta `Análise Sintática`.
