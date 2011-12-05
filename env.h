#pragma once

#include "syntree.h"

/*環境変数の定義*/
typedef struct envs * env_list_t;

typedef struct env * env_t;

struct env{
  char * f; /*変数名*/
  syntree_info_t info; /*格納場所情報*/
};

/*環境変数関連の関数群*/
int cogen_alloc_storage_fun_def(fun_def_t d);
int cogen_alloc_storage_stmt(stmt_t st);
int cogen_alloc_storage_return(stmt_t s);
int cogen_alloc_storage_empty(stmt_t s);
int cogen_alloc_storage_continue(stmt_t s);
int cogen_alloc_storage_break(stmt_t s);
int cogen_alloc_storage_expr(expr_t s);
int cogen_alloc_storage_compound(stmt_t s);
int cogen_alloc_storage_if(stmt_t s);
int cogen_alloc_storage_while(stmt_t s);

env_list_t mk_env(void);
void set_env_params(var_decl_list_t params, env_list_t envs);
void add_env_params(env_list_t envs, env_t env);
int env_list_sz(env_list_t envs);
env_t env_list_get(env_list_t envs, int i);
char * cpy_info(syntree_info_t info);
