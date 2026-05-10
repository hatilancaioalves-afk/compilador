# Análise Léxica

O analisador léxico foi implementado utilizando o **Flex**, ferramenta destinada à geração automática de analisadores léxicos a partir de expressões regulares.

O lexer desenvolvido reconhece os seguintes tokens da linguagem MiniLang:

- Palavras-chave
- Identificadores
- Números inteiros e reais
- Operadores aritméticos e relacionais
- Delimitadores

Durante a execução, o código-fonte é lido caractere por caractere e convertido em uma sequência de tokens.

---

# Código do Analisador Léxico

[Lexer.l](../Análise%20Léxica/Lexer.l)

---

# Como executar no Ubuntu

## 1. Instalação das ferramentas

No Ubuntu, instale o Flex, o GCC e as ferramentas básicas de compilação:

```bash
sudo apt update
sudo apt install flex bison gcc make
```

## 2. Verificação da instalação

Confirme se os programas foram instalados corretamente:

```bash
flex --version
gcc --version
```

## 3. Compilação do lexer

Entre na pasta do projeto e gere o arquivo C a partir do arquivo `.l`:

```bash
cd "~/caminho/para/Análise Léxica"
flex Lexer.l
```

Depois compile o arquivo gerado:

```bash
gcc lex.yy.c -o lexer
```

## 4. Execução do teste

Com o executável pronto, rode o analisador léxico usando o arquivo de teste:

```bash
./lexer < teste.txt
```

## 5. Resultado esperado

O programa deve ler o conteúdo de `teste.txt` e imprimir, linha a linha, os tokens reconhecidos, como palavras-chave, identificadores, números, operadores e delimitadores.



