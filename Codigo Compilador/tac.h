#ifndef TAC_H
#define TAC_H

#include "ast.h"

typedef enum {
	TAC_KIND_ASSIGN,
	TAC_KIND_BINARY,
	TAC_KIND_UNARY,
	TAC_KIND_PRINT,
	TAC_KIND_IF_FALSE,
	TAC_KIND_LABEL
} TACKind;

typedef struct TACInstr {
	TACKind kind;
	char *result;
	char *arg1;
	char *arg2;
	char *op;
	struct TACInstr *next;
} TACInstr;

typedef struct {
	TACInstr *head;
	TACInstr *tail;
} TACProgram;

TACProgram *tac_program_create(void);
TACInstr *tac_instr_create(TACKind kind,
						   const char *result,
						   const char *arg1,
						   const char *arg2,
						   const char *op);
void tac_program_append(TACProgram *program, TACInstr *instr);
void tac_program_free(TACProgram *program);

TACProgram *tac_build(ASTNode *root);
void tac_print(const TACProgram *program, const char *title);

void tac_generate(ASTNode *root);

#endif