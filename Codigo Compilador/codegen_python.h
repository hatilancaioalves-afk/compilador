#ifndef CODEGEN_PYTHON_H
#define CODEGEN_PYTHON_H

#include "ast.h"
#include "tac.h"

void codegen_python_generate(ASTNode *root);
void codegen_python_generate_from_tac(const TACProgram *program);

#endif