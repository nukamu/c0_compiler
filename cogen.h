#pragma once

#include "syntree.h"
#include "env.h"

/*programのアセンブリコード出力関数群*/
void cogen_program(FILE * fp, program_t p);
void cogen_program_header(FILE * fp, char * f);
void cogen_program_trailer(FILE * fp);
void cogen_fun_def(FILE * fp, fun_def_t d);
void cogen_fun_def_header(FILE * fp, char * f);
void cogen_fun_def_trailer(FILE * fp, char * f);
void cogen_prologue(FILE * fp, fun_def_t d, int n);
void cogen_epilogue(FILE * fp, fun_def_t d);

/*stmtのアセンブリコード出力関数群*/
void cogen_stmt(FILE * fp, stmt_t s, env_list_t envs);
void cogen_stmt_return(FILE * fp, stmt_t s, env_list_t envs);
void cogen_stmt_empty(FILE * fp, stmt_t s, env_list_t envs);
void cogen_stmt_continue(FILE * fp, stmt_t s, env_list_t envs);
void cogen_stmt_break(FILE *fp, stmt_t s, env_list_t envs);
void cogen_stmt_expr(FILE *fp, stmt_t s, env_list_t envs);
void cogen_stmt_compound(FILE *fp, stmt_t s, env_list_t envs);
void cogen_stmt_if(FILE * fp, stmt_t s, env_list_t envs);
void cogen_stmt_while(FILE *fp, stmt_t s, env_list_t envs);

/*exprをアセンブリコード出力関数群*/
void cogen_expr(FILE * fp, expr_t e, env_list_t envs);
void cogen_expr_int_literal(FILE * fp, expr_t e, env_list_t envs);
void cogen_expr_id(FILE * fp, expr_t e, env_list_t envs);
void cogen_expr_app(FILE * fp, expr_t e, env_list_t envs);
