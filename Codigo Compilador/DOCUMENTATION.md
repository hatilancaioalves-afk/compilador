**Visão Geral**
- **Propósito do diretório**: contém as etapas finais do compilador (análise léxica, sintática, semântica, geração de AST e código intermediário TAC) implementadas em C usando Flex/Bison.

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
- **Explicação simples**: verifica "faz sentido" o que o programa está pedindo — por exemplo, não deixa somar texto com número ou usar uma variável sem declará-la.
- **Funções principais e comportamentos**:
  - `semantic_init()`: inicia o estado (sem erros e sem tipo corrente).
  - `semantic_set_current_type(type_name)`: guarda o tipo atual quando se está processando uma declaração (por exemplo, depois de `int`).
  - `semantic_declare_identifier(name)`: tenta inserir na tabela de símbolos; marca erro se já existe.
  - `semantic_check_assignment(name, expr_type)`: verifica se a variável existe e se o tipo da expressão é compatível com o tipo da variável.
  - `semantic_check_print(name)`: verifica se a variável passada ao `print` existe.
  - `semantic_type_of_identifier(name)`: retorna o tipo da variável (ou marca erro se não declarada).
  - `semantic_check_arithmetic/relational/logical(...)`: regras que determinam resultado de operações e detectam combinações inválidas (por exemplo, operação aritmética com `string` gera erro; se um lado for `float` o resultado é `float`, senão `int`).
- **Como o código funciona**: guarda um `current_type` temporário enquanto processa declarações; usa a `symbol_table` para checagens; quando encontra inconsistência, imprime mensagem e marca `semantic_error`.

**Geração de Código Intermediário (tac.h / tac.c)**
- **Arquivo**: [tac.h](Codigo%20Intermediario/tac.h) / [tac.c](Codigo%20Intermediario/tac.c)
- **O que faz**: percorre a AST e monta um programa TAC em memória, imprimindo a versão bruta no terminal.
- **Explicação simples**: converte a árvore (estrutura) em linhas simples que descrevem cada cálculo passo a passo — é como transformar uma receita complexa em passos numerados.
- **Funções principais e comportamentos**:
  - `new_temp()` / `new_label()`: geram nomes temporários (`t1`, `t2`) e labels (`L1`) para rótulos de controle.
  - `emit(...)`: imprime uma instrução TAC formatada.
  - `gen_expr(node)`: gera código para expressões; quando encontra operadores cria um `temp` e imprime a linha com a operação.
  - `gen_stmt(node)`: trata comandos (atribuição, print, if, listas de comandos).
  - `tac_build(root)`: monta a lista de instruções TAC em memória a partir da AST.
  - `tac_generate(root)`: mantém compatibilidade e imprime o TAC bruto.
- **Exemplos de saída**: para `x = (a + b) * (c - d)` pode gerar linhas como `t1 = a + b`, `t2 = c - d`, `t3 = t1 * t2`, `x = t3`.

**Otimizador de TAC (optimizer.h / optimizer.c)**
- **Arquivo**: [optimizer.h](Codigo%20Intermediario/optimizer.h) / [optimizer.c](Codigo%20Intermediario/optimizer.c)
- **O que faz**: recebe o TAC bruto e produz uma versão otimizada aplicando propagação de constantes, simplificação algébrica e eliminação de temporários mortos.
- **Pontos principais**:
  - substitui variáveis/temporários conhecidos por constantes quando possível;
  - simplifica expressões como `x * 1`, `x + 0`, `0 * y`, `!False`;
  - remove temporários sem uso posterior;
  - elimina blocos mortos quando a condição do `ifFalse` vira constante.
- **Resultado**: o TAC otimizado alimenta os geradores de Python e C.

**Analisador Léxico (lexer.l / lex.yy.c)**
- **Arquivo**: [lexer.l](Codigo%20Intermediario/lexer.l) e [lex.yy.c](Codigo%20Intermediario/lex.yy.c)
- **O que faz**: lê o texto do programa e separa em "tokens" (palavras com significado, como `int`, `if`, nomes de variáveis, números, operadores).
- **Explicação simples**: como um filtro que lê texto e reconhece palavras-chave, números e símbolos para entregar ao parser.
- **Pontos importantes**:
  - regras para palavras-chave (`int`, `float`, `if`, `print`), literais de string (`'texto'`), identificadores (`LETRA (LETRA|DIGITO)*`), números (inteiro/float), operadores (`+ - * /`), relacionais e lógicos.
  - atribui `yylval.str = strdup(yytext)` para passar o texto/token para o parser.
  - `lex.yy.c` é o arquivo gerado pelo Flex: contém muito código automático; o essencial é que cada regra retorna um código de token que o parser usa.

**Analisador Sintático (parser.y / parser.tab.c / parser.tab.h)**
- **Arquivo**: [parser.y](Codigo%20Intermediario/parser.y) / [parser.tab.c](Codigo%20Intermediario/parser.tab.c) / [parser.tab.h](Codigo%20Intermediario/parser.tab.h)
- **O que faz**: define a gramática da linguagem (como os tokens se combinam para formar comandos válidos) e cria a AST aplicando ações quando regras são reconhecidas.
- **Explicação simples**: o parser pega a lista de tokens do lexer e monta a árvore (AST) seguindo regras (por exemplo: uma atribuição é `IDENTIFICADOR = expressao ;`).
- **Pontos importantes na `parser.y`**:
  - regras para `programa`, `lista_comandos`, `declaracao`, `atribuicao`, `comando_if`, `comando_print`, `expressao`.
  - ações semânticas: ao ver `int a, b;` o parser chama `semantic_set_current_type("int")` e `semantic_declare_identifier("a")`/`("b")`.
  - ao reconhecer expressões o parser cria nós da AST (`criar_no`) com tipos determinados pelas funções semânticas.
  - `main()` inicializa a análise semântica, executa `yyparse()`, imprime o AST e, se não houver erros semânticos, chama `tac_generate(raiz)`.
- **Arquivos `.c` e `.h` com sufixo `tab`**: são produtos gerados pelo Bison; contêm a implementação automatizada do parser e as constantes de token. A versão humana para editar é `parser.y`.

**Arquivo de teste (teste.txt)**
- **Arquivo**: [teste.txt](Codigo%20Intermediario/teste.txt)
- **Conteúdo**:
```
begin {
    int a, b, x, d, c;
    x = (a + b) * (c - d);
} end
```
- **O que demonstra**: declaração de variáveis (`int a, b, x, d, c`) e uma atribuição com expressão composta, cobrindo análise léxica, sintática, semântica e geração de TAC.

**Geradores finais (codegen_python.h / codegen_python.c e codegen_c.h / codegen_c.c)**
- **Arquivo**: [codegen_python.h](Codigo%20Intermediario/codegen_python.h) / [codegen_python.c](Codigo%20Intermediario/codegen_python.c) e [codegen_c.h](Codigo%20Intermediario/codegen_c.h) / [codegen_c.c](Codigo%20Intermediario/codegen_c.c)
- **O que fazem**: consomem o TAC otimizado e escrevem, respectivamente, `programa.py` e `programa.c`.
- **Explicação simples**: transformam o TAC já enxuto em código executável nas linguagens finais.

**Fluxo geral de execução (como tudo se conecta)**
- `lex` (lexer) transforma texto em tokens.
- `yacc/bison` (parser) consome tokens, aplica a gramática e constrói a AST com `criar_no(...)`.
- Durante o parsing, a camada semântica (`semantic.c`) verifica tipos, registra variáveis na tabela (`symbol_table.c`) e marca erros.
- Se não houver erro semântico, `tac_build` cria o TAC bruto, `tac_optimize` gera o TAC otimizado e os geradores finais escrevem `programa.py` e `programa.c`.

**Observações / Limitações**
- A tabela de símbolos usa um array fixo de tamanho limitado (`MAX_SYMBOLS = 100`). Para programas maiores é preciso usar uma estrutura dinâmica (hash table).
- Mensagens de erro semântico são impressas diretamente (não há mecanismo de recuperação avançado).
- `lex.yy.c` e `parser.tab.c` são gerados automaticamente; edite `lexer.l` e `parser.y` e regenere com `flex`/`bison`.

**Como testar rapidamente**
- Na pasta `Codigo Intermediario`, execute:
```bash
flex lexer.l
bison -d parser.y
gcc -o compilador parser.tab.c lex.yy.c ast.c symbol_table.c semantic.c tac.c optimizer.c codegen_python.c codegen_c.c -lfl
./compilador < teste.txt
```

Se quiser, eu posso gerar uma versão mais longa que mostre trechos de código comentados linha-a-linha para qualquer arquivo específico — diga qual arquivo você quer detalhado. 
