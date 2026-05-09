# Introdução

A linguagem escolhida foi a **MiniLang**, por possuir sintaxe simples e comandos básicos inspirados em linguagens como o C.

A MiniLang suporta:

- Declaração de variáveis
- Operações aritméticas
- Estruturas condicionais
- Estruturas de repetição
- Entrada e saída de dados

A MiniLang utiliza o paradigma **imperativo/estruturado**, no qual o programa é executado sequencialmente, linha por linha.

---

# Estrutura da Linguagem

Os programas desenvolvidos em MiniLang seguem a seguinte estrutura:

- Os comandos são delimitados por `{ }`

## Tipos de Dados

A linguagem possui os seguintes tipos de dados:

- `int` → números inteiros
- `float` → números reais
- `string` → textos
- `bool` → valores booleanos

---

# Variáveis

As variáveis devem ser declaradas antes do uso.

## Exemplos

```txt
int idade;
float salario;
```

---

# Operadores Aritméticos

A MiniLang suporta os seguintes operadores aritméticos:

| Operador | Função |
|---|---|
| `+` | Soma |
| `-` | Subtração |
| `*` | Multiplicação |
| `/` | Divisão |

---

# Operadores Relacionais

| Operador | Significado |
|---|---|
| `==` | Igual |
| `!=` | Diferente |
| `>` | Maior |
| `<` | Menor |
| `>=` | Maior ou igual |
| `<=` | Menor ou igual |

---

# Operadores Lógicos

| Operador | Significado |
|---|---|
| `&&` | E lógico |
| `||` | OU lógico |

---

# Estrutura Condicional — if else

Permite executar comandos com base em uma condição.

## Exemplo

```txt
if (condicao) {
    comandos
} else {
    comandos
}
```

---

# Estrutura de Repetição — for

A linguagem possui suporte à estrutura de repetição `for`.

## Exemplo

```txt
for (inicializacao; condicao; incremento) {
    comandos
}
```

---

# Saída de Dados

A função `print()` é utilizada para exibir valores na tela.

## Exemplo

```txt
print("Olá Mundo");
```

---

# Simplificações da Linguagem

Algumas funcionalidades da linguagem original foram simplificadas, como a redução das estruturas de repetição disponíveis.

Essas modificações foram realizadas para facilitar o desenvolvimento e a implementação do compilador durante o projeto da disciplina.