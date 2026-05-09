# Gramática Formal da MiniLang (EBNF)

```ebnf
programa        ::= "begin" "{" { declaracao | comando } "}" "end" ;

declaracao      ::= tipo identificador ";" ;

tipo            ::= "int"
                  | "float"
                  | "string"
                  | "bool" ;

comando         ::= atribuicao
                  | condicional
                  | repeticao
                  | escrita ;

atribuicao      ::= identificador "=" expressao ";" ;

condicao        ::= expressao operador_relacional expressao ;

operador_relacional ::= "=="
                      | "!="
                      | ">"
                      | "<"
                      | ">="
                      | "<=" ;

condicional     ::= "if" "(" condicao ")" "{"
                        { comando }
                    "}"
                    [ "else" "{"
                        { comando }
                    "}" ] ;

repeticao       ::= "for"
                    "(" atribuicao condicao ";" atribuicao ")"
                    "{"
                        { comando }
                    "}" ;

escrita         ::= "print" "(" expressao ")" ";" ;

expressao       ::= termo
                    { ("+" | "-") termo } ;

termo           ::= fator
                    { ("*" | "/") fator } ;

fator           ::= numero
                  | identificador
                  | "(" expressao ")" ;

identificador   ::= letra
                    { letra | digito | "_" } ;

numero          ::= digito
                    { digito }
                    [ "." digito { digito } ] ;

letra           ::= "a" | ... | "z"
                  | "A" | ... | "Z" ;

digito          ::= "0" | "1" | "2" | "3" | "4"
                  | "5" | "6" | "7" | "8" | "9" ;
```

---

# Autômato Finito para Reconhecimento de Identificadores

```txt
[a-zA-Z_][a-zA-Z0-9_]*
```

![Automato 01](./Imagens/Automato%2001.png)

O autômato acima representa o reconhecimento de identificadores da linguagem.

Um identificador deve iniciar com uma letra (`A-Z`, `a-z`) ou sublinhado (`_`).

Após o primeiro caractere, o identificador pode conter:

- Letras
- Números (`0-9`)
- Sublinhados (`_`)

---

# Autômato Finito para Reconhecimento de Números

```txt
[0-9]+(\.[0-9]+)?
```

![Automato 02](./Imagens/Automato%2002.png)

O autômato acima reconhece números inteiros e números decimais da linguagem MiniLang.

- O estado `q1` representa números inteiros válidos
- O estado `q3` representa números decimais válidos

O autômato exige pelo menos um dígito antes e depois do ponto decimal.

---

# Paradigma da Linguagem

A MiniLang utiliza os paradigmas **imperativo** e **estruturado**.

---

## Paradigma Imperativo

No paradigma imperativo, o programa é executado por meio de comandos sequenciais que alteram o estado das variáveis durante a execução.

A linguagem utiliza atribuições e instruções executadas passo a passo.

### Exemplo

```txt
x = 10;
x = x + 1;
```

Nesse exemplo, o valor da variável `x` é modificado durante a execução do programa.

---

## Paradigma Estruturado

A MiniLang também segue o paradigma estruturado, utilizando estruturas de controle para organizar o fluxo de execução do programa.

As principais estruturas utilizadas são:

- Condicionais (`if` / `else`)
- Estruturas de repetição (`for`)
- Blocos delimitados por `{ }`

Esse modelo facilita a leitura, organização e manutenção do código.