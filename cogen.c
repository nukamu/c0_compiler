#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "syntree.h"
#include "cogen.h"
#include "env.h"

void cogen_program(FILE * fp, program_t p){
  cogen_program_header(fp, p->filename);
  fun_def_list_t ds = p->fun_defs;
  int n = fun_def_list_sz(ds);
  int i;
  for(i = 0; i < n; i++){
    fun_def_t d = fun_def_list_get(ds, i);
    cogen_fun_def(fp, d);
  }
  cogen_program_trailer(fp);
}

void cogen_program_header(FILE * fp, char * f){
  /* output header of program */
  fprintf(fp, "\t.file\t\"%s\"\n", f);
  fprintf(fp, "\t.text\n");
}

void cogen_program_trailer(FILE * fp){
  /* output footer of program */
  fprintf(fp, "\t.ident\t\"GCC: (Ubuntu 4.3.3-5ubuntu4) 4.3.3\"\n");
  fprintf(fp, "\t.section\t.note.GNU-stack,\"\",@progbits\n");
}

void cogen_fun_def(FILE * fp, fun_def_t d){
  /* decide locations each data stored at */
  int frame_sz = cogen_alloc_storage_fun_def(d);
  /* make env used in functions */
  env_list_t envs = mk_env();
  /* register data locations of args to env */
  set_env_params(d->params, envs);

  /* output header of function */
  cogen_fun_def_header(fp, d->f);
  cogen_prologue(fp, d, frame_sz);
  /* body of function */
  cogen_stmt(fp, d->body, envs);
  /* output footer of function */
  cogen_epilogue(fp, d);
  cogen_fun_def_trailer(fp, d->f);
}

void cogen_fun_def_header(FILE * fp, char * f){
  /* output header of function */
  fprintf(fp, ".globl %s\n", f);
  fprintf(fp, "\t.type\t%s, @function\n", f);
  fprintf(fp, "%s:\n", f);
}

void cogen_fun_def_trailer(FILE *fp, char * f){
  /* output footer of function */
  fprintf(fp, "\t.size\t%s, .-%s\n", f, f);
}

void cogen_prologue(FILE * fp, fun_def_t d, int n){
  /* cogen prologue of function */
  fprintf(fp, "\tpushl\t%%ebp\n");
  fprintf(fp, "\tmovl\t%%esp, %%ebp\n");
  if(n > 0){
    fprintf(fp,"\tsubl\t$%d, %%esp\n", n * 4);
  }
}

void cogen_epilogue(FILE * fp, fun_def_t d){
  /* cogen epilogue of function */
  fprintf(fp, "\tleave\n");
  fprintf(fp, "\tret\n");
}

void cogen_stmt(FILE * fp, stmt_t s, env_list_t envs){
  switch(s->kind){
  case stmt_kind_return:
    cogen_stmt_return(fp, s, envs);
    break;
  case stmt_kind_empty:
    cogen_stmt_empty(fp, s, envs);
    break;
  case stmt_kind_continue:
    cogen_stmt_continue(fp, s, envs);
    break;
  case stmt_kind_break:
    cogen_stmt_break(fp, s, envs);
    break;
  case stmt_kind_expr:
    cogen_stmt_expr(fp, s, envs);
    break;
  case stmt_kind_compound:
    cogen_stmt_compound(fp, s, envs);
    break;
  case stmt_kind_if:
    cogen_stmt_if(fp, s, envs);
    break;
  case stmt_kind_while:
    cogen_stmt_while(fp, s, envs);
    break;
  default:
    assert(0);
  }
}

void cogen_stmt_return(FILE * fp, stmt_t t, env_list_t envs){
}

void cogen_stmt_empty(FILE * fp, stmt_t t, env_list_t envs){
}

void cogen_stmt_continue(FILE * fp, stmt_t t, env_list_t envs){

}

void cogen_stmt_break(FILE * fp, stmt_t t, env_list_t envs){

}

void cogen_stmt_expr(FILE * fp, stmt_t t, env_list_t envs){

}

void cogen_stmt_compound(FILE * fp, stmt_t t, env_list_t envs){

}

void cogen_stmt_if(FILE * fp, stmt_t t, env_list_t envs){
}

void cogen_stmt_while(FILE * fp, stmt_t t, env_list_t envs){


}

void cogen_expr(FILE *fp, expr_t e, env_list_t envs){
  switch (e->kind) {
  case expr_kind_int_literal:
    cogen_expr_int_literal(fp, e, envs);
    break;
  case expr_kind_id:
    cogen_expr_id(fp, e, envs);
    break;
  case expr_kind_paren:
    cogen_expr(fp, e->u.p, envs);
    break;
  case expr_kind_app:
    cogen_expr_app(fp, e, envs);
    break;
  default:
    assert(0);
  }
}

void cogen_expr_int_literal(FILE * fp, expr_t e, env_list_t envs){

}

void cogen_expr_id(FILE * fp, expr_t e, env_list_t envs){

}

void cogen_expr_app(FILE * fp, expr_t e, env_list_t envs){


}
