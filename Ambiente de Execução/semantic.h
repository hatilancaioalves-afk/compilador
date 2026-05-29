#ifndef SEMANTIC_H
#define SEMANTIC_H

void semantic_init(void);
void semantic_set_current_type(const char *type_name);
void semantic_declare_identifier(char *name);
void semantic_check_assignment(char *name, const char *expr_type);
void semantic_check_print(char *name);
const char *semantic_type_of_identifier(char *name);
const char *semantic_check_arithmetic(const char *left_type, const char *right_type);
const char *semantic_check_relational(const char *left_type, const char *right_type);
const char *semantic_check_logical(const char *left_type, const char *right_type);
int semantic_has_error(void);

#endif