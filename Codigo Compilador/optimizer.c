#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "optimizer.h"

typedef struct {
    char *name;
    char *value;
} ConstEntry;

typedef struct {
    ConstEntry *items;
    size_t count;
    size_t capacity;
} ConstEnv;

static char *dup_string(const char *text) {
    char *copy = malloc(strlen(text) + 1);

    if(copy == NULL)
        return NULL;

    strcpy(copy, text);
    return copy;
}

static int is_constant_value(const char *text) {
    if(text == NULL || *text == '\0')
        return 0;

    if(strcmp(text, "True") == 0 || strcmp(text, "False") == 0)
        return 1;

    if(text[0] == '"')
        return 1;

    char *end = NULL;
    strtod(text, &end);
    return end != NULL && *end == '\0';
}

static int is_true_constant(const char *text) {
    if(text == NULL)
        return 0;

    if(strcmp(text, "False") == 0 || strcmp(text, "0") == 0)
        return 0;

    return is_constant_value(text);
}

static int is_temp_name(const char *name) {
    return name != NULL && name[0] == 't' && isdigit((unsigned char)name[1]);
}

static void env_init(ConstEnv *env) {
    env->items = NULL;
    env->count = 0;
    env->capacity = 0;
}

static void env_free(ConstEnv *env) {
    if(env == NULL)
        return;

    for(size_t i = 0; i < env->count; i++) {
        free(env->items[i].name);
        free(env->items[i].value);
    }

    free(env->items);
    env->items = NULL;
    env->count = 0;
    env->capacity = 0;
}

static ConstEnv env_clone(const ConstEnv *source) {
    ConstEnv clone;
    env_init(&clone);

    for(size_t i = 0; i < source->count; i++) {
        if(clone.count == clone.capacity) {
            size_t new_capacity = clone.capacity == 0 ? 16 : clone.capacity * 2;
            ConstEntry *grown = realloc(clone.items, new_capacity * sizeof(ConstEntry));

            if(grown == NULL)
                break;

            clone.items = grown;
            clone.capacity = new_capacity;
        }

        clone.items[clone.count].name = dup_string(source->items[i].name);
        clone.items[clone.count].value = dup_string(source->items[i].value);
        clone.count++;
    }

    return clone;
}

static const char *env_get(const ConstEnv *env, const char *name) {
    for(size_t i = 0; i < env->count; i++) {
        if(strcmp(env->items[i].name, name) == 0)
            return env->items[i].value;
    }

    return NULL;
}

static void env_remove(ConstEnv *env, const char *name) {
    for(size_t i = 0; i < env->count; i++) {
        if(strcmp(env->items[i].name, name) == 0) {
            free(env->items[i].name);
            free(env->items[i].value);
            env->items[i] = env->items[env->count - 1];
            env->count--;
            return;
        }
    }
}

static void env_set(ConstEnv *env, const char *name, const char *value) {
    for(size_t i = 0; i < env->count; i++) {
        if(strcmp(env->items[i].name, name) == 0) {
            free(env->items[i].value);
            env->items[i].value = dup_string(value);
            return;
        }
    }

    if(env->count == env->capacity) {
        size_t new_capacity = env->capacity == 0 ? 16 : env->capacity * 2;
        ConstEntry *grown = realloc(env->items, new_capacity * sizeof(ConstEntry));

        if(grown == NULL)
            return;

        env->items = grown;
        env->capacity = new_capacity;
    }

    env->items[env->count].name = dup_string(name);
    env->items[env->count].value = dup_string(value);
    env->count++;
}

static char *format_number(double value, int keep_float) {
    char buffer[64];

    if(keep_float)
        snprintf(buffer, sizeof(buffer), "%g", value);
    else
        snprintf(buffer, sizeof(buffer), "%lld", (long long)value);

    return dup_string(buffer);
}

static char *substitute_operand(const ConstEnv *env, const char *operand) {
    const char *known = env_get(env, operand);
    return dup_string(known != NULL ? known : operand);
}

static char *fold_unary(const char *op, const char *value) {
    if(strcmp(op, "-") == 0 && is_constant_value(value))
        return format_number(-strtod(value, NULL), strchr(value, '.') != NULL);

    if(strcmp(op, "!") == 0) {
        if(strcmp(value, "True") == 0 || strcmp(value, "1") == 0)
            return dup_string("False");

        if(strcmp(value, "False") == 0 || strcmp(value, "0") == 0)
            return dup_string("True");
    }

    return NULL;
}

static char *fold_binary(const char *op, const char *left, const char *right) {
    int left_const = is_constant_value(left);
    int right_const = is_constant_value(right);

    if(left_const && right_const) {
        if(strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || strcmp(op, "*") == 0 || strcmp(op, "/") == 0) {
            double l = strtod(left, NULL);
            double r = strtod(right, NULL);
            double result = 0.0;
            int keep_float = strchr(left, '.') != NULL || strchr(right, '.') != NULL || strcmp(op, "/") == 0;

            if(strcmp(op, "+") == 0)
                result = l + r;
            else if(strcmp(op, "-") == 0)
                result = l - r;
            else if(strcmp(op, "*") == 0)
                result = l * r;
            else if(strcmp(op, "/") == 0)
                result = r == 0.0 ? 0.0 : l / r;

            return format_number(result, keep_float);
        }

        if(strcmp(op, "&&") == 0 || strcmp(op, "||") == 0) {
            int l = is_true_constant(left);
            int r = is_true_constant(right);
            int result = strcmp(op, "&&") == 0 ? (l && r) : (l || r);
            return dup_string(result ? "True" : "False");
        }

        if(strcmp(op, "==") == 0 || strcmp(op, "!=") == 0 || strcmp(op, "<") == 0 ||
           strcmp(op, "<=") == 0 || strcmp(op, ">") == 0 || strcmp(op, ">=") == 0) {
            double l = strtod(left, NULL);
            double r = strtod(right, NULL);
            int result = 0;

            if(strcmp(op, "==") == 0)
                result = l == r;
            else if(strcmp(op, "!=") == 0)
                result = l != r;
            else if(strcmp(op, "<") == 0)
                result = l < r;
            else if(strcmp(op, "<=") == 0)
                result = l <= r;
            else if(strcmp(op, ">") == 0)
                result = l > r;
            else if(strcmp(op, ">=") == 0)
                result = l >= r;

            return dup_string(result ? "True" : "False");
        }
    }

    if(strcmp(op, "+") == 0) {
        if(strcmp(left, "0") == 0)
            return dup_string(right);
        if(strcmp(right, "0") == 0)
            return dup_string(left);
    }

    if(strcmp(op, "-") == 0) {
        if(strcmp(right, "0") == 0)
            return dup_string(left);
    }

    if(strcmp(op, "*") == 0) {
        if(strcmp(left, "0") == 0 || strcmp(right, "0") == 0)
            return dup_string("0");
        if(strcmp(left, "1") == 0)
            return dup_string(right);
        if(strcmp(right, "1") == 0)
            return dup_string(left);
    }

    if(strcmp(op, "/") == 0) {
        if(strcmp(left, "0") == 0)
            return dup_string("0");
        if(strcmp(right, "1") == 0)
            return dup_string(left);
    }

    if(strcmp(op, "&&") == 0) {
        if(strcmp(left, "False") == 0 || strcmp(left, "0") == 0 || strcmp(right, "False") == 0 || strcmp(right, "0") == 0)
            return dup_string("False");
        if(strcmp(left, "True") == 0)
            return dup_string(right);
        if(strcmp(right, "True") == 0)
            return dup_string(left);
    }

    if(strcmp(op, "||") == 0) {
        if(strcmp(left, "True") == 0 || strcmp(right, "True") == 0)
            return dup_string("True");
        if(strcmp(left, "False") == 0)
            return dup_string(right);
        if(strcmp(right, "False") == 0)
            return dup_string(left);
    }

    return NULL;
}

static void append_cloned(TACProgram *program, const TACInstr *instr) {
    tac_program_append(program, tac_instr_create(instr->kind, instr->result, instr->arg1, instr->arg2, instr->op));
}

static const TACInstr *skip_block(const TACInstr *current, const char *stop_label) {
    while(current != NULL) {
        if(current->kind == TAC_KIND_LABEL && current->result != NULL && strcmp(current->result, stop_label) == 0)
            return current->next;

        if(current->kind == TAC_KIND_IF_FALSE)
            current = skip_block(current->next, current->result);
        else
            current = current->next;
    }

    return NULL;
}

static const TACInstr *optimize_sequence(const TACInstr *current,
                                         const char *stop_label,
                                         TACProgram *out,
                                         ConstEnv *env,
                                         int *encountered_stop) {
    *encountered_stop = 0;

    while(current != NULL) {
        if(stop_label != NULL && current->kind == TAC_KIND_LABEL && current->result != NULL && strcmp(current->result, stop_label) == 0) {
            *encountered_stop = 1;
            return current->next;
        }

        if(current->kind == TAC_KIND_IF_FALSE) {
            char *condition = substitute_operand(env, current->arg1);

            if(is_true_constant(condition)) {
                int body_stop = 0;
                current = optimize_sequence(current->next, current->result, out, env, &body_stop);
                free(condition);
                continue;
            }

            if(strcmp(condition, "False") == 0 || strcmp(condition, "0") == 0) {
                current = skip_block(current->next, current->result);
                free(condition);
                continue;
            }

            ConstEnv snapshot = env_clone(env);
            append_cloned(out, tac_instr_create(TAC_KIND_IF_FALSE, current->result, condition, NULL, NULL));

            int body_stop = 0;
            current = optimize_sequence(current->next, current->result, out, env, &body_stop);
            append_cloned(out, tac_instr_create(TAC_KIND_LABEL, current != NULL && body_stop ? stop_label : current == NULL ? current->result : stop_label, NULL, NULL, NULL));

            env_free(env);
            *env = snapshot;
            free(condition);
            continue;
        }

        if(current->kind == TAC_KIND_ASSIGN || current->kind == TAC_KIND_BINARY || current->kind == TAC_KIND_UNARY || current->kind == TAC_KIND_PRINT) {
            char *arg1 = current->arg1 != NULL ? substitute_operand(env, current->arg1) : NULL;
            char *arg2 = current->arg2 != NULL ? substitute_operand(env, current->arg2) : NULL;

            if(current->kind == TAC_KIND_ASSIGN) {
                if(arg1 != NULL && is_constant_value(arg1))
                    env_set(env, current->result, arg1);
                else
                    env_remove(env, current->result);

                append_cloned(out, tac_instr_create(TAC_KIND_ASSIGN, current->result, arg1 != NULL ? arg1 : current->arg1, NULL, NULL));
            } else if(current->kind == TAC_KIND_UNARY) {
                char *folded = arg1 != NULL ? fold_unary(current->op, arg1) : NULL;

                if(folded != NULL) {
                    if(is_constant_value(folded))
                        env_set(env, current->result, folded);
                    append_cloned(out, tac_instr_create(TAC_KIND_ASSIGN, current->result, folded, NULL, NULL));
                    free(folded);
                } else {
                    env_remove(env, current->result);
                    append_cloned(out, tac_instr_create(TAC_KIND_UNARY, current->result, arg1 != NULL ? arg1 : current->arg1, NULL, current->op));
                }
            } else if(current->kind == TAC_KIND_BINARY) {
                char *folded = (arg1 != NULL && arg2 != NULL) ? fold_binary(current->op, arg1, arg2) : NULL;

                if(folded != NULL) {
                    if(is_constant_value(folded))
                        env_set(env, current->result, folded);
                    append_cloned(out, tac_instr_create(TAC_KIND_ASSIGN, current->result, folded, NULL, NULL));
                    free(folded);
                } else {
                    env_remove(env, current->result);
                    append_cloned(out, tac_instr_create(TAC_KIND_BINARY, current->result, arg1 != NULL ? arg1 : current->arg1,
                                                        arg2 != NULL ? arg2 : current->arg2,
                                                        current->op));
                }
            } else if(current->kind == TAC_KIND_PRINT) {
                append_cloned(out, tac_instr_create(TAC_KIND_PRINT, NULL, arg1 != NULL ? arg1 : current->arg1, NULL, NULL));
            }

            free(arg1);
            free(arg2);
            current = current->next;
            continue;
        }

        append_cloned(out, current);
        current = current->next;
    }

    return current;
}

static size_t count_uses(const TACProgram *program, const char *name) {
    size_t uses = 0;

    for(const TACInstr *instr = program->head; instr != NULL; instr = instr->next) {
        if(instr->arg1 != NULL && strcmp(instr->arg1, name) == 0)
            uses++;
        if(instr->arg2 != NULL && strcmp(instr->arg2, name) == 0)
            uses++;
    }

    return uses;
}

static TACProgram *eliminate_dead_temporaries(const TACProgram *program) {
    TACProgram *result = tac_program_create();

    if(result == NULL)
        return NULL;

    int changed;

    do {
        changed = 0;
        TACProgram *next = tac_program_create();

        if(next == NULL) {
            tac_program_free(result);
            return NULL;
        }

        for(const TACInstr *instr = program->head; instr != NULL; instr = instr->next) {
            if((instr->kind == TAC_KIND_ASSIGN || instr->kind == TAC_KIND_BINARY || instr->kind == TAC_KIND_UNARY) &&
               is_temp_name(instr->result) && count_uses(program, instr->result) == 0) {
                changed = 1;
                continue;
            }

            append_cloned(next, instr);
        }

        tac_program_free(result);
        result = next;
        program = result;
    } while(changed);

    return result;
}

TACProgram *tac_optimize(const TACProgram *program) {
    TACProgram *optimized = tac_program_create();

    if(program == NULL || optimized == NULL)
        return optimized;

    ConstEnv env;
    env_init(&env);

    int stopped = 0;
    optimize_sequence(program->head, NULL, optimized, &env, &stopped);

    env_free(&env);

    TACProgram *final_program = eliminate_dead_temporaries(optimized);
    tac_program_free(optimized);
    return final_program;
}