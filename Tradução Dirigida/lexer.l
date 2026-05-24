%{
#include <stdio.h>
#include <string.h>
#include "parser.tab.h"
%}

%option noyywrap

DIGITO      [0-9]
LETRA       [a-zA-Z_]

%%

"int"              { return INT; }
"float"            { return FLOAT; }
"string"           { return STRING; }
"bool"             { return BOOL; }

"if"               { return IF; }
"else"             { return ELSE; }
"for"              { return FOR; }

"print"            { return PRINT; }

"begin"            { return BEGINN; }
"end"              { return END; }

'[^']*'           { yylval.str = strdup(yytext); return STRING_LITERAL; }

{LETRA}({LETRA}|{DIGITO})*  { yylval.str = strdup(yytext); return IDENTIFICADOR; }
{DIGITO}+(\.{DIGITO}+)?     { yylval.str = strdup(yytext); return NUMERO; }

"=="|"!="|">="|"<="|">"|"<"  { return OP_REL; }
"&&"|"||"|"!"                 { return OP_LOGICO; }
"="                            { return ATRIB; }
"+"|"-"|"*"|"/"              { return OP_ARIT; }
","                            { return VIRGULA; }
";"                            { return PONTO_VIRGULA; }
"("                            { return ABRE_PAR; }
")"                            { return FECHA_PAR; }
"{"                            { return ABRE_CHAVE; }
"}"                            { return FECHA_CHAVE; }

[ \t\r\n]+                     { /* ignora espaços */ }
.                                { printf("TOKEN INVALIDO: %s\n", yytext); }

%%
