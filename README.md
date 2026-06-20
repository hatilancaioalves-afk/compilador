# Mini compilador

Este projeto é um mini compilador desenvolvido para a disciplina de Compiladores.
Ele faz análise léxica, sintática e semântica, gera código intermediário em TAC, aplica otimizações e produz saídas em Python.

## Como compilar

Na pasta `Codigo Compilador`, compile com:

```bash
gcc -o compilador parser.tab.c lex.yy.c ast.c symbol_table.c semantic.c tac.c optimizer.c codegen_python.c -lfl
```

## Como executar

O compilador lê a entrada pela entrada padrão. Para testar com o arquivo de exemplo:

```bash
./compilador < teste.txt
```

## Saídas geradas

Quando a análise termina sem erros, o projeto gera os arquivos `programa.py` e `programa.c` na mesma pasta.

## Observação

Se você alterar a gramática no arquivo `parser.y` ou o analisador léxico em `lexer.l`, será necessário regenerar `parser.tab.c`, `parser.tab.h` e `lex.yy.c` antes de recompilar.