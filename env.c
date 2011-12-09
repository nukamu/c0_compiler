#include "util.h"
#include "env.h"
#include <string.h>
#include <assert.h>

/*式の結果の格納場所、変数の格納場所を決定する*/
int cogen_alloc_storage_fun_def(fun_def_t d){
  stmt_t st = d->body;
  return cogen_alloc_storage_stmt(st);
}

int cogen_alloc_storage_stmt(stmt_t st){
  switch(st->kind){
  case stmt_kind_return:
    return cogen_alloc_storage_return(st);
  case stmt_kind_empty:
    return cogen_alloc_storage_empty(st);
  case stmt_kind_continue:
    return cogen_alloc_storage_continue(st);
  case stmt_kind_break:
    return cogen_alloc_storage_break(st);
  case stmt_kind_expr:
    return cogen_alloc_storage_expr(st->u.e);
  case stmt_kind_compound:
    return cogen_alloc_storage_compound(st);
  case stmt_kind_if:
    return cogen_alloc_storage_if(st);
  case stmt_kind_while:
    return cogen_alloc_storage_while(st);
  default:
    assert(0);
  }
}

int cogen_alloc_storage_return(stmt_t s){
  expr_t ex = s->u.e;
  return cogen_alloc_storage_expr(ex);
}

int cogen_alloc_storage_empty(stmt_t s){
  return 0;
}

int cogen_alloc_storage_continue(stmt_t s){
  return 0;
}

int cogen_alloc_storage_break(stmt_t s){
  return 0;
}

int cogen_alloc_storage_expr(expr_t s){
  return 0;
}

int cogen_alloc_storage_compound(stmt_t s){
  return 0;
}

int cogen_alloc_storage_if(stmt_t s){
  return 0;
}

int cogen_alloc_storage_while(stmt_t s){
  return 0;
}

/*envを作る*/
env_list_t mk_env(void){
  return (env_list_t)mk_list();
}

env_t mk_env_entry(void){
  return (env_t)safe_malloc(sizeof(struct env));
}

/*引数の割り当て場所を環境に登録*/
void set_env_params(var_decl_list_t params, env_list_t envs){
  char * a = (char *)malloc(sizeof(char) * 10);
  strcpy(a, "ebp");
  int sz = var_decl_list_sz(params);
  int i;
  int num = 4;
  syntree_info_t info = (syntree_info_t)safe_malloc(sizeof(struct syntree_info));
  info->kind = place_mem;
  info->a = a;
  for (i = 0; i < sz; i++) {
    num = num + 4;
    info->num = num;
    var_decl_t d = var_decl_list_get(params, i);
    env_t env = mk_env_entry();
    env->f = d->v;
    env->info = info;
    add_env_params(envs, env);
  }
}

/*envをリストに追加*/
void add_env_params(env_list_t envs, env_t v){
  list_add((list_t)envs, (void *)v);
}

/*envsの個数を返す*/
int env_list_sz(env_list_t envs){
  return list_sz((list_t)envs);
}

/*envsのi番目の要素を取得*/
env_t env_list_get(env_list_t envs, int i){
  return (env_t)list_get((list_t)envs, i);
}

/*envから変数を検索*/



/*infoから場所文字列を作る*/
char * cpy_info(syntree_info_t info){
  char * a = (char *)safe_malloc(sizeof(char) * 10);
  switch(info->kind){
  case place_redi:
    sprintf(a, "%%%s", info->a);
    break;
  case place_mem:
    sprintf(a, "%d(%%%s)", info->num, info->a);
    break;
  }
  return a;
}

