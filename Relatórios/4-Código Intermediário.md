# Código Intermediário (Three-Address Code)

Este relatório descreve o formato textual do Código Intermediário (Three-Address Code — TAC) gerado pelo módulo de "Código Intermediário" do compilador MiniLang implementado neste trabalho.

O objetivo é documentar convenções, formato das instruções, mapeamento das construções da AST para TAC, exemplos e instruções de uso para gerar e testar o código intermediário.

---

# 1. Visão Geral

O TAC é uma representação linear e textual das operações do programa, onde a maioria das instruções tem no máximo três operandos (resultado, operando1, operando2). O gerador atual produz TAC legível para facilitar testes, depuração e posteriores transformações/otimizações.

---

# 2. Formato das instruções

- Atribuição binária:

  `result = op1 OP op2`

  Exemplo: `t1 = a + b`

- Atribuição unária:

  `result = OP op1`

  Exemplo: `t1 = -a`

- Atribuição direta (atribuição de literal/temporário para variável):

  `var = value`

  Exemplo: `idade = 20`

- Impressão:

  `print X`

  Exemplo: `print idade` ou `print t3`

- Desvio condicional:

  `ifFalse temp goto Lx`

  (salta para o rótulo `Lx` se o valor em `temp` for falso)

- Rótulo:

  `Lx:`

- Desvio incondicional (opcional):

  `goto Lx`

---

# 3. Convenções

- Temporários: `t1, t2, t3, ...` — gerados incrementalmente.
- Rótulos: `L1, L2, L3, ...` — usados para controle de fluxo (if/else/loops).
- Operadores: escritos com o símbolo lido pelo analisador (`+`, `-`, `*`, `/`, `==`, `>=`, `&&`, `||`, etc.).
- Cada instrução ocupa uma linha.
- Operandos podem ser: identificadores, literais (números, strings) ou temporários.

---

# 4. Mapeamento AST → TAC (resumo)

- Expressões aritméticas/relacionais/lógicas: geração recursiva — cada operação gera um temporário que representa seu resultado.
- Atribuição: gera código para a expressão RHS, obtém temporário resultado e emite `LHS = temp`.
- Print: gera código para o argumento (se necessário) e emite `print <var|temp>`.
- If sem else:
  - gerar condição → `temp`
  - `ifFalse temp goto Lx`
  - [código do then]
  - `Lx:`
- If-else:
  - gerar condição → `temp`
  - `ifFalse temp goto Lelse`
  - [then]
  - `goto Lend`
  - `Lelse:`
  - [else]
  - `Lend:`
- Loops: usar rótulos para início/condição/saída e `ifFalse` para sair do loop.

---

# 5. Exemplo (saída gerada pelo gerador atual)

Entrada (exemplo simplificado):

```minilang
begin
    int idade;
    idade = 20;
    if (idade >= 18 && idade != 0) then
        print idade;
    end
end
```

TAC gerado (exemplo):

```tac
idade = 20
t1 = idade >= 18
t2 = idade != 0
t3 = t1 && t2
ifFalse t3 goto L1
print idade
L1:
```

---

# 6. Observações de implementação

- O gerador atual está implementado em um módulo separado (`tac.c`/`tac.h`) que recebe a AST construída pelo parser e percorre-a gerando instruções TAC.
- A geração é feita em um passe sobre a AST, preservando a ordem das operações. Isso facilita futuras otimizações (eliminação de temporários, alocação de registradores, etc.).
- Declarações: no estado atual, declarações são registradas na tabela de símbolos para verificação semântica, mas não geram instruções de alocação explícita no TAC textual. Caso queira emitir directives de alocação, sugere-se adicionar um formato, por exemplo: `var x : int`.

---

# 7. Como gerar e testar (exemplo)

1. Entre na pasta do módulo de código intermediário:

```bash
cd "Codigo Intermediario"
```

2. Gere o parser e o lexer (se necessário):

```bash
bison -d parser.y
flex lexer.l
```

3. Compile:

```bash
gcc parser.tab.c lex.yy.c ast.c semantic.c tac.c symbol_table.c -o compilador -lfl
```

4. Execute com um arquivo de teste:

```bash
./compilador < teste.txt
```

O programa exibirá mensagens semânticas, a impressão da AST (por convenção) e o TAC gerado.

---

# 8. Extensões sugeridas

- Emitir diretivas de declaração no TAC (ex.: `var x : int`).
- Gerar comentário com referência à linha do código-fonte (`# linha N`).
- Implementar um otimizador simples (eliminação de temporários temporários, propagação de constantes).
- Suportar saída em arquivo (`.tac`) para posterior consumo por um backend/gerador de código final.

---

# 9. Referências de implementação

- Código do gerador TAC: `Codigo Intermediario/tac.c`, `Codigo Intermediario/tac.h`
- AST: `Codigo Intermediario/ast.c`, `Codigo Intermediario/ast.h`
- Parser e lexer: `Codigo Intermediario/parser.y`, `Codigo Intermediario/lexer.l`
- Tabela de símbolos e semântica: `Codigo Intermediario/symbol_table.c`, `Codigo Intermediario/semantic.c`



---

*Relatório gerado automaticamente por auxílio do ambiente de desenvolvimento.*
