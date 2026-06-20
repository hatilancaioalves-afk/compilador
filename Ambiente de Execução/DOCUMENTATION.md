**Visão Geral**
- **Propósito do diretório**: contém as etapas finais do compilador (análise léxica, sintática, semântica, geração de AST, código intermediário TAC e simulação do ambiente de execução) implementadas em C usando Flex/Bison.

**Arquivos e explicações (para leigos)**

**AST (ast.h / ast.c)**
- **Arquivo**: [ast.h](Codigo%20Intermediario/ast.h) / [ast.c](Codigo%20Intermediario/ast.c)
- **O que faz**: define e constrói a Árvore Sintática Abstrata (AST), que é uma forma estruturada de representar o código fonte como uma árvore.
- **Explicação simples**: imagine uma árvore onde cada nó é uma operação (por exemplo, soma) ou um valor (número, identificador). O compilador transforma o texto em árvore para entender a ordem das operações.
- **Funções principais**:
  - `ASTNode` (struct): armazena tipo do nó (`tipo_no`), valor textual (`valor`), tipo de dado (`tipo`) e ponteiros para filhos esquerdo/direito.
  - `criar_no(...)`: aloca um novo nó, copia as strings recebidas e liga os filhos.
  - `imprimir_ast(...)`: imprime a árvore de forma legível em formato de texto com ramificações.
- **Como o código funciona**: `criar_no` usa `malloc` para reservar memória e `strcpy` para copiar labels; `imprimir_ast` percorre a árvore recursivamente e desenha a estrutura no terminal.

**Tabela de Símbolos (symbol_table.h / symbol_table.c)**
- **Arquivo**: [symbol_table.h](Codigo%20Intermediario/symbol_table.h) / [symbol_table.c](Codigo%20Intermediario/symbol_table.c)
- **O que faz**: guarda informações sobre variáveis declaradas (nome e tipo) durante a análise semântica.
- **Explicação simples**: pense numa lista onde o compilador anota cada variável que você declara, para depois verificar usos e tipos.
- **Funções principais**:
  - `inserir_simbolo(nome, tipo)`: adiciona uma variável; avisa se já existe.
  - `simbolo_existe(nome)`: verifica se já foi declarada.
  - `obter_tipo(nome)`: retorna o tipo armazenado para a variável.
- **Como o código funciona**: usa um array fixo `tabela[MAX_SYMBOLS]` e um contador `total_simbolos`. Ao inserir, copia nome e tipo; ao buscar, percorre linearmente.

**Análise Semântica (semantic.h / semantic.c)**
- **Arquivo**: [semantic.h](Codigo%20Intermediario/semantic.h) / [semantic.c](Codigo%20Intermediario/semantic.c)
- **O que faz**: aplica regras de tipo e consistência depois que a árvore sintática foi construída (por exemplo, checa se variáveis foram declaradas, se tipos nas atribuições batem, se operações são válidas).
- **Explicação simples**: verifica se "faz sentido" o que o programa está pedindo — por exemplo, não deixa somar texto com número ou usar uma variável sem declará-la.
- **Funções principais e comportamentos**:
  - `semantic_init()`: inicia o estado (sem erros e sem tipo corrente).
  - `semantic_set_current_type(type_name)`: guarda o tipo atual quando se está processando uma declaração (por exemplo, depois de `int`).
  - `semantic_declare_identifier(name)`: tenta inserir na tabela de símbolos; marca erro se já existe.
  - `semantic_check_assignment(name, expr_type)`: verifica se a variável existe e se o tipo da expressão é compatível com o tipo da variável.
  - `semantic_check_print(name)`: verifica se a variável passada ao `print` existe.
  - `semantic_type_of_identifier(name)`: retorna o tipo da variável (ou marca erro se não declarada).
  - `semantic_check_arithmetic/relational/logical(...)`: regras que determinam resultado de operações e detectam combinações inválidas.
- **Como o código funciona**: guarda um `current_type` temporário enquanto processa declarações; usa a `symbol_table` para checagens; quando encontra inconsistência, imprime mensagem e marca `semantic_error`.

**Geração de Código Intermediário (tac.h / tac.c)**
- **Arquivo**: [tac.h](Codigo%20Intermediario/tac.h) / [tac.c](Codigo%20Intermediario/tac.c)
- **O que faz**: percorre a AST e gera TAC (Three-Address Code), uma forma simples e linear de representar operações (ex.: t1 = a + b; t2 = t1 * c;). Imprime o TAC no terminal.
- **Explicação simples**: converte a árvore (estrutura) em linhas simples que descrevem cada cálculo passo a passo — é como transformar uma receita complexa em passos numerados.
- **Funções principais e comportamentos**:
  - `new_temp()` / `new_label()`: geram nomes temporários (`t1`, `t2`) e labels (`L1`) para rótulos de controle.
  - `emit(...)`: imprime uma instrução TAC formatada.
  - `gen_expr(node)`: gera código para expressões; quando encontra operadores cria um `temp` e imprime a linha com a operação.
  - `gen_stmt(node)`: trata comandos (atribuição, print, if, funções, return, listas de comandos).
  - `tac_generate(root)`: inicializa contadores e dispara a geração a partir da raiz da AST.

**Ambiente de Execução (runtime.h / runtime.c)**
- **Arquivo**: [runtime.h](Codigo%20Intermediario/runtime.h) / [runtime.c](Codigo%20Intermediario/runtime.c)
- **O que faz**: Simula o gerenciamento de memória durante a execução do programa compilado, controlando o escopo de funções através de uma Pilha de Execução e Registros de Ativação (Activation Records).
- **Explicação simples**: Funciona como a memória de curto prazo do programa. Quando uma função é chamada, o sistema empilha uma "caixa" (registro) para organizar as variáveis locais e saber para onde voltar. Quando a função termina, ele devolve o resultado e destrói essa caixa (desempilha).
- **Funções e Estruturas principais**:
  - `ActivationRecord` (struct): Guarda as informações do contexto atual (nome da função, valor de retorno e um ponteiro de link dinâmico para a função que a chamou).
  - `pilha_push(nome_funcao, endereco)`: Aloca um novo registro no topo da pilha simulando o início de uma função e gera um log `[call]`.
  - `pilha_pop()`: Remove o registro do topo da pilha simulando o comando de retorno, limpando a memória e gerando um log `[return]`.
- **Exemplo de saída**: Ao chamar uma função e voltar ao bloco principal, gera logs lineares como `[call] soma`, `[return] soma = 5`, `[end] main()`.

**Analisador Léxico (lexer.l / lex.yy.c)**
- **Arquivo**: [lexer.l](Codigo%20Intermediario/lexer.l) e [lex.yy.c](Codigo%20Intermediario/lex.yy.c)
- **O que faz**: lê o texto do programa e separa em "tokens" (palavras com significado, como `int`, `return`, nomes de variáveis, números, operadores).
- **Explicação simples**: como um filtro que lê texto e reconhece palavras-chave, números e símbolos para entregar ao parser.
- **Pontos importantes**:
  - `lex.yy.c` é o arquivo gerado pelo Flex: contém muito código automático; o essencial é que cada regra retorna um código de token que o parser usa.

**Analisador Sintático (parser.y / parser.tab.c / parser.tab.h)**
- **Arquivo**: [parser.y](Codigo%20Intermediario/parser.y) / [parser.tab.c](Codigo%20Intermediario/parser.tab.c) / [parser.tab.h](Codigo%20Intermediario/parser.tab.h)
- **O que faz**: define a gramática da linguagem (como os tokens se combinam para formar comandos válidos) e cria a AST aplicando ações quando regras são reconhecidas.
- **Explicação simples**: o parser pega a lista de tokens do lexer e monta a árvore (AST) seguindo regras (por exemplo: uma atribuição é `IDENTIFICADOR = expressao ;`).
- **Pontos importantes na `parser.y`**:
  - regras para funções, bloco principal, `return` e expressões avançadas.
  - ações semânticas integradas durante a formação da árvore.
  - `main()` inicializa a análise, gera o TAC e dispara a simulação da memória baseada no `runtime.c`.

**Arquivo de teste (teste.txt)**
- **Arquivo**: [teste.txt](Codigo%20Intermediario/teste.txt)
- **Conteúdo atualizado**:
```c
int soma(int a, int b) {
    int r;
    r = a + b;
    return r;
}

beginn {
    int x;
    x = soma(2, 3);
    print x;
} end