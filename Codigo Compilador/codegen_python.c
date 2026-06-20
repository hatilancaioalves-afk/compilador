#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "codegen_python.h"
#include "semantic.h"
#include "symbol_table.h"

static FILE *output_file = NULL;
static int current_indent = 0;

static char *dup_string(const char *text) {
    char *copy = malloc(strlen(text) + 1);
    if(copy == NULL)
        return NULL;

    strcpy(copy, text);
    return copy;
}

static void write_indent(void) {
    for(int i = 0; i < current_indent; i++)
        fputs("    ", output_file);
}

static void emit_line(const char *format, ...) {
    va_list args;

    write_indent();

    va_start(args, format);
    vfprintf(output_file, format, args);
    va_end(args);

    fputc('\n', output_file);
}

static int is_kind(ASTNode *node, const char *kind) {
    return node != NULL && strcmp(node->tipo_no, kind) == 0;
}

static const char *python_default_for_type(const char *type_name) {
    if(type_name == NULL)
        return "None";

    if(strcmp(type_name, "int") == 0)
        return "0";

    if(strcmp(type_name, "float") == 0)
        return "0.0";

    if(strcmp(type_name, "string") == 0)
        return "\"\"";

    if(strcmp(type_name, "bool") == 0)
        return "False";

    return "None";
}

static char *escape_python_string(const char *text) {
    size_t length = strlen(text);
    int has_quotes = length >= 2 && ((text[0] == '"' && text[length - 1] == '"') ||
                                     (text[0] == '\'' && text[length - 1] == '\''));

    if(has_quotes)
        return dup_string(text);

    size_t capacity = length * 2 + 3;
    char *escaped = malloc(capacity);
    if(escaped == NULL)
        return NULL;

    char *cursor = escaped;
    *cursor++ = '"';

    for(size_t i = 0; i < length; i++) {
        if(text[i] == '\\' || text[i] == '"')
            *cursor++ = '\\';

        *cursor++ = text[i];
    }

    *cursor++ = '"';
    *cursor = '\0';
    return escaped;
}

static char *gen_expr(ASTNode *node);

static char *make_binary_expression(const char *left, const char *op, const char *right) {
    size_t length = strlen(left) + strlen(op) + strlen(right) + 5;
    char *expression = malloc(length);

    if(expression == NULL)
        return NULL;

    snprintf(expression, length, "(%s %s %s)", left, op, right);
    return expression;
}

static char *gen_expr(ASTNode *node) {
    if(node == NULL)
        return dup_string("None");

    if(is_kind(node, "numero") || is_kind(node, "identificador"))
        return dup_string(node->valor);

    if(is_kind(node, "string"))
        return escape_python_string(node->valor);

    if(is_kind(node, "operacao")) {
        char *left = gen_expr(node->esquerda);
        char *right = gen_expr(node->direita);
        char *result = NULL;

        if(left == NULL || right == NULL) {
            free(left);
            free(right);
            return NULL;
        }

        if(node->direita == NULL || strcmp(node->valor, "!") == 0) {
            const char *operator_name = strcmp(node->valor, "!") == 0 ? "not" : node->valor;
            size_t length = strlen(operator_name) + strlen(left) + 4;
            result = malloc(length);

            if(result != NULL)
                snprintf(result, length, "(%s %s)", operator_name, left);
        } else {
            const char *python_op = node->valor;

            if(strcmp(node->valor, "&&") == 0)
                python_op = "and";
            else if(strcmp(node->valor, "||") == 0)
                python_op = "or";

            result = make_binary_expression(left, python_op, right);
        }

        free(left);
        free(right);
        return result;
    }

    return dup_string("None");
}

static void gen_stmt(ASTNode *node);

static void gen_list(ASTNode *node) {
    if(node == NULL)
        return;

    if(is_kind(node, "lista_comandos")) {
        gen_list(node->esquerda);
        gen_list(node->direita);
        return;
    }

    gen_stmt(node);
}

static void emit_symbol_initializations(void) {
    for(int i = 0; i < total_simbolos; i++)
        emit_line("%s = %s", tabela[i].nome, python_default_for_type(tabela[i].tipo));

    if(total_simbolos > 0)
        fputc('\n', output_file);
}

typedef struct {
    char *label;
    int body_emitted;
} PythonIfFrame;

static void close_if_block(PythonIfFrame *frames, size_t *frame_count) {
    if(*frame_count == 0)
        return;

    if(!frames[*frame_count - 1].body_emitted)
        emit_line("pass");

    current_indent--;
    free(frames[*frame_count - 1].label);
    (*frame_count)--;
}

void codegen_python_generate_from_tac(const TACProgram *program) {
    if(semantic_has_error()) {
        printf("Erro semantico detectado; programa.py nao gerado.\n");
        return;
    }

    output_file = fopen("programa.py", "w");

    if(output_file == NULL) {
        printf("Erro ao gerar programa.py\n");
        return;
    }

    current_indent = 0;
    fprintf(output_file, "# Arquivo gerado automaticamente a partir de MiniLang\n\n");
    emit_symbol_initializations();

    PythonIfFrame frames[64];
    size_t frame_count = 0;

    for(const TACInstr *instr = program == NULL ? NULL : program->head; instr != NULL; instr = instr->next) {
        if(instr->kind == TAC_KIND_LABEL) {
            while(frame_count > 0) {
                if(strcmp(frames[frame_count - 1].label, instr->result) == 0) {
                    close_if_block(frames, &frame_count);
                    break;
                }

                close_if_block(frames, &frame_count);
            }
            continue;
        }

        if(instr->kind == TAC_KIND_IF_FALSE) {
            char *condition = instr->arg1;
            emit_line("if not %s:", condition);
            if(frame_count < 64) {
                frames[frame_count].label = dup_string(instr->result);
                frames[frame_count].body_emitted = 0;
                frame_count++;
            }
            current_indent++;
            continue;
        }

        if(frame_count > 0)
            frames[frame_count - 1].body_emitted = 1;

        if(instr->kind == TAC_KIND_ASSIGN) {
            emit_line("%s = %s", instr->result, instr->arg1);
            continue;
        }

        if(instr->kind == TAC_KIND_BINARY) {
            emit_line("%s = %s %s %s", instr->result, instr->arg1, instr->op, instr->arg2);
            continue;
        }

        if(instr->kind == TAC_KIND_UNARY) {
            if(strcmp(instr->op, "!") == 0)
                emit_line("%s = not %s", instr->result, instr->arg1);
            else
                emit_line("%s = %s%s", instr->result, instr->op, instr->arg1);
            continue;
        }

        if(instr->kind == TAC_KIND_PRINT) {
            emit_line("print(%s)", instr->arg1);
            continue;
        }
    }

    while(frame_count > 0)
        close_if_block(frames, &frame_count);

    fclose(output_file);
    output_file = NULL;

    printf("\nPython gerado em programa.py\n");
}

static void gen_stmt(ASTNode *node) {
    if(node == NULL)
        return;

    if(is_kind(node, "lista_comandos")) {
        gen_list(node);
        return;
    }

    if(is_kind(node, "declaracao"))
        return;

    if(is_kind(node, "atribuicao")) {
        char *expr = gen_expr(node->esquerda);

        if(expr != NULL) {
            emit_line("%s = %s", node->valor, expr);
            free(expr);
        }

        return;
    }

    if(is_kind(node, "print")) {
        emit_line("print(%s)", node->valor);
        return;
    }

    if(is_kind(node, "if")) {
        char *condition = gen_expr(node->esquerda);

        if(condition != NULL) {
            emit_line("if %s:", condition);
            free(condition);

            current_indent++;
            gen_stmt(node->direita);
            current_indent--;
        }

        return;
    }

    gen_list(node);
}

void codegen_python_generate(ASTNode *root) {
    if(semantic_has_error()) {
        printf("Erro semantico detectado; programa.py nao gerado.\n");
        return;
    }

    output_file = fopen("programa.py", "w");

    if(output_file == NULL) {
        printf("Erro ao gerar programa.py\n");
        return;
    }

    current_indent = 0;

    fprintf(output_file, "# Arquivo gerado automaticamente a partir de MiniLang\n\n");
    emit_symbol_initializations();
    gen_stmt(root);

    fclose(output_file);
    output_file = NULL;

    printf("\nPython gerado em programa.py\n");
}