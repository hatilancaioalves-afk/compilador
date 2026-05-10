%{
#include <stdio.h>
%}

%option noyywrap

DIGITO      [0-9]
LETRA       [a-zA-Z_]

%%

"int"|"float"|"string"|"bool"|"if"|"else"|"for"|"print"|"begin"|"end" { printf("PALAVRA-CHAVE: %s\n", yytext); }

{LETRA}({LETRA}|{DIGITO})* { printf("IDENTIFICADOR: %s\n", yytext); }

{DIGITO}+(\.{DIGITO}+)? { printf("NUMERO: %s\n", yytext); }

"=="|"!="|">="|"<="|">"|"<" { printf("OPERADOR RELACIONAL: %s\n", yytext); }

"&&"|"||" { printf("OPERADOR LOGICO: %s\n", yytext); }

"+"|"-"|"*"|"/"|"=" { printf("OPERADOR ARITMETICO: %s\n", yytext); }

";"|"("|")"|"{"|"}" { printf("DELIMITADOR: %s\n", yytext); }

[ \t\r\n]+ { /* Ignora espaços em branco */ }

. { printf("TOKEN INVALIDO: %s\n", yytext); }

%%

int main() {
    yylex();
    return 0;
}