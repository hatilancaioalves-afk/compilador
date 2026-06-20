#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "codegen_c.h"
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

static const char *c_type_for_symbol(const char *type_name) {
    if(type_name == NULL)
        return "int";

    if(strcmp(type_name, "float") == 0)
        return "double";

    if(strcmp(type_name, "string") == 0)
        return "char *";

    return "int";
}

static const char *c_default_for_type(const char *type_name) {
    if(type_name == NULL)
        return "0";

    if(strcmp(type_name, "float") == 0)
        return "0.0";

    if(strcmp(type_name, "string") == 0)
        return "\"\"";

    return "0";
}

static const char *c_printf_for_type(const char *type_name) {
    if(type_name == NULL)
        return "%d";

    if(strcmp(type_name, "float") == 0)
        return "%f";

    if(strcmp(type_name, "string") == 0)
        return "%s";

    return "%d";
}

typedef struct {
    char *label;
} CIfFrame;

static void close_if_block(CIfFrame *frames, size_t *frame_count) {
    if(*frame_count == 0)
        return;

    current_indent--;
    emit_line("}");
    free(frames[*frame_count - 1].label);
    (*frame_count)--;
}

void codegen_c_generate_from_tac(const TACProgram *program) {
    if(semantic_has_error()) {
        printf("Erro semantico detectado; programa.c nao gerado.\n");
        return;
    }

    output_file = fopen("programa.c", "w");

    if(output_file == NULL) {
        printf("Erro ao gerar programa.c\n");
        return;
    }

    current_indent = 0;

    fprintf(output_file, "#include <stdio.h>\n\n");
    fprintf(output_file, "int main(void) {\n");
    current_indent++;

    for(int i = 0; i < total_simbolos; i++)
        emit_line("%s %s = %s;", c_type_for_symbol(tabela[i].tipo), tabela[i].nome, c_default_for_type(tabela[i].tipo));

    if(total_simbolos > 0)
        fputc('\n', output_file);

    CIfFrame frames[64];
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
            emit_line("if (!(%s)) {", instr->arg1);

            if(frame_count < 64)
                frames[frame_count++].label = dup_string(instr->result);

            current_indent++;
            continue;
        }

        if(instr->kind == TAC_KIND_ASSIGN) {
            emit_line("%s = %s;", instr->result, instr->arg1);
            continue;
        }

        if(instr->kind == TAC_KIND_BINARY) {
            emit_line("%s = %s %s %s;", instr->result, instr->arg1, instr->op, instr->arg2);
            continue;
        }

        if(instr->kind == TAC_KIND_UNARY) {
            if(strcmp(instr->op, "!") == 0)
                emit_line("%s = !%s;", instr->result, instr->arg1);
            else
                emit_line("%s = %s%s;", instr->result, instr->op, instr->arg1);
            continue;
        }

        if(instr->kind == TAC_KIND_PRINT) {
            const char *type_name = obter_tipo(instr->arg1);
            emit_line("printf(\"%s\\n\", %s);", c_printf_for_type(type_name), instr->arg1);
            continue;
        }
    }

    while(frame_count > 0)
        close_if_block(frames, &frame_count);

    emit_line("return 0;");
    current_indent--;
    fprintf(output_file, "}\n");

    fclose(output_file);
    output_file = NULL;

    printf("\nC gerado em programa.c\n");
}