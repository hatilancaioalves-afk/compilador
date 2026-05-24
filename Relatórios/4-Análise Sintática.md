# Análise Sintática

A análise sintática da MiniLang tem como objetivo validar se a sequência de tokens gerada pelo analisador léxico segue corretamente a gramática definida para a linguagem.

O parser verifica a estrutura do programa, identificando possíveis erros sintáticos, como:

- Falta de delimitadores
- Expressões inválidas
- Estruturas de controle incorretas
- Erros de atribuição

---

# Técnica Utilizada

O parser da MiniLang foi implementado utilizando o GNU Bison, ferramenta destinada à geração automática de analisadores sintáticos a partir de gramáticas formais.

A implementação utiliza gramáticas livres de contexto para definir as regras sintáticas da linguagem MiniLang.

Nesse modelo:

- A análise é realizada sobre a sequência de tokens produzida pelo analisador léxico
- As produções gramaticais definem a estrutura válida da linguagem
- O parser utiliza regras sintáticas para reconhecer comandos e expressões

Essa abordagem foi escolhida por possuir integração direta com o analisador léxico desenvolvido em Flex e por facilitar a construção de compiladores acadêmicos e educacionais.

---

# Funcionamento do Parser

O parser recebe os tokens produzidos pelo analisador léxico e realiza a validação sintática do programa utilizando as regras definidas no arquivo `parser.y`.

Cada produção da gramática representa uma construção válida da linguagem.

Durante a análise, o parser verifica estruturas como:

- Declarações de variáveis
- Comandos de atribuição
- Expressões aritméticas
- Expressões relacionais
- Expressões lógicas
- Estruturas condicionais
- Estruturas de repetição
- Blocos de comandos

Caso a estrutura analisada esteja incorreta, o parser emite mensagens de erro sintático.

---

# Estrutura Validada

O parser reconhece:

- Declarações de variáveis
- Atribuições
- Estruturas condicionais (`if` / `else`)
- Estruturas de repetição (`for`)
- Expressões aritméticas
- Expressões relacionais
- Expressões lógicas
- Comandos de saída (`print`)

---

# Exemplo de Código Válido

```txt
int idade;
float salario;

idade = 20;

if (idade >= 18 && idade != 0) {
    print idade;
}
```

---

# Árvore Sintática Abstrata (AST)

Após a validação sintática, o parser possibilita a construção de uma Árvore Sintática Abstrata (AST).

A AST representa a estrutura lógica do programa de forma hierárquica.

Diferente da árvore sintática completa, a AST remove elementos desnecessários da gramática, mantendo apenas as informações relevantes para as próximas etapas do compilador.

---

# Exemplo de AST

## Código Fonte

```txt
x = 10 + 5;
```

---

## Representação da AST

```txt
        (=)
       /   \
     (x)   (+)
           / \
        (10) (5)
```

---

# Objetivo da AST

A AST será utilizada posteriormente para:

- Análise semântica
- Verificação de tipos
- Geração de código intermediário
- Otimizações
- Geração de código final

---

# Estrutura Geral do Parser

O parser foi organizado em módulos responsáveis por:

| Módulo | Função |
|---|---|
| Lexer | Geração de tokens |
| Parser | Validação sintática |
| AST | Construção da árvore sintática |
| Tabela de Símbolos | Controle de variáveis e tipos |

---

# Vantagens da Abordagem Utilizada

A utilização do Bison para construção do parser apresenta as seguintes vantagens:

- Automatização da análise sintática
- Facilidade de manutenção da gramática
- Boa organização das regras sintáticas
- Integração com o analisador léxico desenvolvido em Flex
- Facilidade de expansão da linguagem
- Suporte à construção de ASTs e ações semânticas

---

# Código do Parser

`parser.y`

---

# Como Executar no Ubuntu

## 1. Instalação das ferramentas

No Ubuntu, instale o Flex, o Bison, o GCC e as ferramentas básicas de compilação:

```bash
sudo apt update
sudo apt install flex bison gcc make
```

---

## 2. Verificação da instalação

Confirme se as ferramentas foram instaladas corretamente:

```bash
flex --version
bison --version
gcc --version
```

---

## 3. Geração do parser

Entre na pasta do projeto e gere os arquivos do parser a partir do arquivo `.y`:

```bash
cd "~/caminho/para/AnaliseSintatica"
bison -d parser.y
```

Esse comando irá gerar os arquivos:

```txt
parser.tab.c
parser.tab.h
```

---

## 4. Geração do analisador léxico

Agora gere o lexer utilizando o Flex:

```bash
flex lexer.l
```

Esse comando irá gerar o arquivo:

```txt
lex.yy.c
```

---

## 5. Compilação do projeto

Compile o lexer e o parser utilizando o GCC:

```bash
gcc lex.yy.c parser.tab.c -o compilador
```

---

## 6. Execução do parser

Execute o compilador utilizando um arquivo de teste contendo código MiniLang:

```bash
./compilador < teste.txt
```

---

# Exemplo de Arquivo de Teste

## teste.txt

```txt
int idade;
float salario;

idade = 20;

if (idade >= 18 && idade != 0) {
    print idade;
}
```

---

# Resultado Esperado

Caso o código esteja sintaticamente correto, o parser deverá concluir a análise sem apresentar erros sintáticos.

Em caso de inconsistências na estrutura do programa, o compilador exibirá mensagens indicando os erros encontrados durante a análise.