# Tradução Dirigida por Sintaxe

A Tradução Dirigida por Sintaxe foi implementada utilizando ações semânticas integradas ao parser desenvolvido com **Bison**.

Essa etapa do compilador é responsável por associar significado às estruturas reconhecidas pela análise sintática, permitindo realizar validações semânticas e manipular informações necessárias para as próximas fases da compilação.

Durante a análise, o parser executa ações semânticas diretamente nas produções da gramática, possibilitando:

- Inserção de identificadores na tabela de símbolos
- Verificação de variáveis declaradas
- Verificação de compatibilidade de tipos
- Controle semântico de expressões
- Identificação de erros semânticos

A implementação utiliza uma tabela de símbolos para armazenar informações sobre variáveis declaradas na linguagem MiniLang.

Para cada identificador são armazenadas informações como:

- Nome da variável
- Tipo da variável
- Informações semânticas utilizadas durante a compilação

Além disso, a análise semântica realiza verificações como:

- Uso de variáveis não declaradas
- Redeclaração de variáveis
- Operações inválidas entre tipos incompatíveis
- Atribuições incompatíveis

---

# Estrutura Implementada

A Tradução Dirigida por Sintaxe foi integrada ao parser através de ações semânticas em regras da gramática.

Exemplo de ação semântica utilizada:

```c
atribuicao:

      IDENTIFICADOR ATRIB expressao PONTO_VIRGULA
      {
          if(!simbolo_existe($1)) {

              printf("Erro semântico: variável '%s' não declarada\n",
                     $1);
          }
      }
;
```

Nesse exemplo, o compilador verifica se a variável utilizada na atribuição já foi declarada anteriormente.

---

# Tabela de Símbolos

A tabela de símbolos foi implementada em linguagem C utilizando estruturas e vetores.

Ela é responsável por armazenar os identificadores reconhecidos durante a compilação.

Exemplo simplificado da estrutura utilizada:

```c
typedef struct {

    char nome[50];
    char tipo[20];

} Simbolo;
```

---

# Verificações Semânticas

A implementação atual realiza as seguintes verificações:

---

## Variáveis não declaradas

```c
x = 10;
```

Resultado:

```text
Erro semântico: variável 'x' não declarada
```

---

## Redeclaração de variáveis

```c
int x;
int x;
```

Resultado:

```text
Erro semântico: 'x' já declarado
```

---

## Incompatibilidade de tipos

```c
string nome;

nome = 10;
```

Resultado:

```text
Erro semântico: tipos incompatíveis em atribuição
```

---

## Operações inválidas

```c
'abc' + 10
```

Resultado:

```text
Erro semântico: operação aritmética inválida com string
```

---

# Código da Tradução Dirigida

- `Parser.y`
- `SymbolTable.c`
- `SymbolTable.h`

---

# Como executar no Ubuntu

## 1. Instalação das ferramentas

No Ubuntu, instale o Flex, o Bison, o GCC e as ferramentas básicas de compilação:

```bash
sudo apt update
sudo apt install flex bison gcc make
```

---

## 2. Verificação da instalação

Confirme se os programas foram instalados corretamente:

```bash
flex --version
bison --version
gcc --version
```

---

## 3. Geração do parser e lexer

Entre na pasta do projeto:

```bash
cd "~/caminho/para/Análise Sintática"
```

Gere o parser com Bison:

```bash
bison -d parser.y
```

Gere o lexer com Flex:

```bash
flex Lexer.l
```

---

## 4. Compilação do projeto

Compile todos os arquivos:

```bash
gcc parser.tab.c lex.yy.c symbol_table.c -o compilador
```

---

## 5. Execução do compilador

Execute utilizando um arquivo de teste:

```bash
./compilador < teste.txt
```

---

## 6. Resultado esperado

O compilador deverá:

- Reconhecer corretamente os comandos da linguagem
- Inserir variáveis na tabela de símbolos
- Validar tipos em expressões e atribuições
- Detectar erros semânticos
- Exibir mensagens de erro quando necessário