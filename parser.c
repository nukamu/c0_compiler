#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "syntree.h"
#include "tokenizer.h"


void eat_it(tokenizer_t t, token_kind_t kind){
  if(cur_tok(t).kind == kind){
    next_tok(t);
  }
  else{
    //printf("exit\n");
    exit(1);
  }
}

program_t parse_program(tokenizer_t t){
  char * filename = cur_tok(t).filename;
  fun_def_list_t l = mk_fun_def_list();
  fun_def_t x;
  while(t->tok.kind != TOK_EOF){
    x = parse_fun_difinition(t);
    fun_def_list_add(l, x);
  }
  return mk_program(filename, l);
}

fun_def_t parse_fun_difinition(tokenizer_t t){
  char * filename = cur_tok(t).filename;
  int line = cur_tok(t).line_num;
  parse_type_expr(t);
  char *f0;
  char *f;
  f0 = parse_identifier(t);
  f = (char *)malloc(sizeof(char) * strlen(f0) + 1);
  strcpy(f, f0);
  eat_it(t, TOK_LPAREN);
  var_decl_list_t param = parse_para_list(t);
  eat_it(t, TOK_RPAREN);
  stmt_t body = parse_comp_stmt(t);
  return mk_fun_def(filename, line, f, param, body);
}

char * parse_identifier(tokenizer_t t){
  char * f = (char *)malloc(sizeof(char) * strlen(cur_tok(t).a) + 1);
  strcpy(f, cur_tok(t).a);
  eat_it(t, TOK_ID);
  return f;
}

void parse_type_expr(tokenizer_t t){
  eat_it(t, TOK_INT);
}

var_decl_list_t parse_para_list(tokenizer_t t){
  var_decl_list_t param = mk_var_decl_list();
  if(cur_tok(t).kind == TOK_RPAREN){
  }
  else if(cur_tok(t).kind == TOK_INT){
    var_decl_t x = parse_para(t);
    var_decl_list_add(param, x);
    while(cur_tok(t).kind == TOK_COMMA){
      eat_it(t, TOK_COMMA);
      x = parse_para(t);
      var_decl_list_add(param, x);
    }
  }
  return param;
}

var_decl_t parse_para(tokenizer_t t){
  char * filename = cur_tok(t).filename;
  int line = cur_tok(t).line_num;
  eat_it(t, TOK_INT);
  char * f0 = parse_identifier(t);
  char *f = (char *)malloc(sizeof(char) * strlen(f0) + 1);
  strcpy(f, f0);
  return mk_var_decl(filename, line, f);
}

stmt_t parse_comp_stmt(tokenizer_t t){
  char * filename = cur_tok(t).filename;
  int line = cur_tok(t).line_num;
  eat_it(t, TOK_LBRACE);
  stmt_list_t c = mk_stmt_list();
  var_decl_list_t d = parse_var_decl(t);
  stmt_t x;
  while(cur_tok(t).kind != TOK_RBRACE){
    x = parse_stmt(t);
    stmt_list_add(c, x);
  }
  eat_it(t, TOK_RBRACE);
  return mk_stmt_compound(filename, line, d, c);
}

var_decl_list_t parse_var_decl(tokenizer_t t){
  var_decl_list_t d = mk_var_decl_list(); 
  char * filename = cur_tok(t).filename;
  int line = cur_tok(t).line_num;
  while(cur_tok(t).kind ==  TOK_INT){
    parse_type_expr(t);
    char * f =(char *)malloc(sizeof(char) * 100);
    strcpy(f, parse_identifier(t));
    var_decl_t x = mk_var_decl(filename, line, f);
    var_decl_list_add(d, x);
    eat_it(t, TOK_SEMICOLON);
  }
  return d;
}

stmt_t parse_stmt(tokenizer_t t){
  char * filename = cur_tok(t).filename;
  int line = cur_tok(t).line_num;
  expr_t e;
  switch(cur_tok(t).kind){
  case TOK_SEMICOLON:
    eat_it(t, TOK_SEMICOLON);
    return mk_stmt_empty(filename, line);
    break;
  case TOK_CONTINUE:
    eat_it(t, TOK_CONTINUE);
    eat_it(t, TOK_SEMICOLON);
    return mk_stmt_continue(filename, line);
    break;
  case TOK_BREAK:
    eat_it(t, TOK_BREAK);
    eat_it(t, TOK_SEMICOLON);
    return mk_stmt_break(filename, line);
    break;
  case TOK_RETURN:
    eat_it(t, TOK_RETURN);
    e = parse_expr(t);
    eat_it(t, TOK_SEMICOLON);
    return mk_stmt_return(filename, line, e);
    break;
  case TOK_IF:
    return parse_stmt_if(t);
    break;
  case TOK_WHILE:
    return parse_stmt_while(t);
    break;
  case TOK_LBRACE:
    return parse_comp_stmt(t);
    break;
  default:
    e = parse_expr(t);
    eat_it(t, TOK_SEMICOLON);
    return mk_stmt_expr(filename, line, e);
    break;
  }
}

stmt_t parse_stmt_if(tokenizer_t t){
  char * filename = cur_tok(t).filename;
  int line = cur_tok(t).line_num;
  eat_it(t, TOK_IF);
  eat_it(t, TOK_LPAREN);
  expr_t e = parse_expr(t);
  eat_it(t, TOK_RPAREN);
  stmt_t th = parse_stmt(t);
  stmt_t el = NULL;
  if(cur_tok(t).kind == TOK_ELSE){
    eat_it(t, TOK_ELSE);
    el = parse_stmt(t);
  }
  return mk_stmt_if(filename, line, e, th, el);
}

stmt_t parse_stmt_while(tokenizer_t t){
  char *filename = cur_tok(t).filename;
  int line_num = cur_tok(t).line_num;
  eat_it(t, TOK_WHILE);
  eat_it(t, TOK_LPAREN);
  expr_t e = parse_expr(t);
  eat_it(t, TOK_RPAREN);
  stmt_t body = parse_stmt(t);
  return mk_stmt_while(filename, line_num, e, body);
}

expr_t parse_expr(tokenizer_t t){
  char * filename = cur_tok(t).filename;
  int line = cur_tok(t).line_num;
  expr_t e0 = parse_equal_expr(t);
  if(cur_tok(t).kind == TOK_ASSIGN){
    eat_it(t, TOK_ASSIGN);
    expr_t e1 = parse_equal_expr(t);
    return mk_expr_bin_op(filename, line, op_kind_assign, e0, e1);
  }
  else{
    return e0;
  }
}

expr_t parse_equal_expr(tokenizer_t t){
  char * filename = cur_tok(t).filename;
  int line = cur_tok(t).line_num;
  expr_t e = parse_relat_expr(t);
  while((cur_tok(t).kind == TOK_EQ)|(cur_tok(t).kind == TOK_NEQ)){
    op_kind_t op = parse_equal_oper(t);
    expr_t e0 = parse_relat_expr(t);
    e = mk_expr_bin_op(filename, line, op, e, e0);
  }
  return e;
}

expr_t parse_relat_expr(tokenizer_t t){
  char * filename = cur_tok(t).filename;
  int line = cur_tok(t).line_num;
  expr_t e = parse_add_expr(t);
  while((cur_tok(t).kind==TOK_LT)|(cur_tok(t).kind==TOK_GT)|(cur_tok(t).kind==TOK_LE)|(cur_tok(t).kind==TOK_GE)){
    op_kind_t op = parse_relat_oper(t);
    expr_t e0 = parse_add_expr(t);
    e = mk_expr_bin_op(filename, line, op, e, e0);
  }
  return e;
}

op_kind_t parse_equal_oper(tokenizer_t t){
  if(cur_tok(t).kind == TOK_EQ){
    eat_it(t, TOK_EQ);
    return op_kind_eq;
  }
  else if(cur_tok(t).kind == TOK_NEQ){
    eat_it(t, TOK_NEQ);
    return op_kind_neq;
  }
  else{
    printf("exit_1\n");
    exit(1);
  }
}

expr_t parse_add_expr(tokenizer_t t){
  char * filename = cur_tok(t).filename;
  int line = cur_tok(t).line_num;
  expr_t e = parse_multi_expr(t);
  while((cur_tok(t).kind == TOK_PLUS)|(cur_tok(t).kind == TOK_MINUS)){
    op_kind_t op = parse_add_oper(t);
    expr_t e0 = parse_multi_expr(t);
    e = mk_expr_bin_op(filename, line, op, e, e0);
  }
  return e;
}

op_kind_t parse_relat_oper(tokenizer_t t){
  if(cur_tok(t).kind == TOK_LT){
    eat_it(t, TOK_LT);
    return op_kind_lt;
  }
  else if(cur_tok(t).kind == TOK_GT){
    eat_it(t, TOK_GT);
    return op_kind_gt;
  }
  else if(cur_tok(t).kind == TOK_LE){
    eat_it(t, TOK_LE);
    return op_kind_le;
  }
  else if(cur_tok(t).kind == TOK_GE){
    eat_it(t, TOK_GE);
    return op_kind_ge;
  }
  else{
    printf("exit_2\n");
    exit(1);
  }
}

expr_t parse_multi_expr(tokenizer_t t){
  char * filename = cur_tok(t).filename;
  int line = cur_tok(t).line_num;
  expr_t e = parse_unary_expr(t);
  while((cur_tok(t).kind == TOK_MUL)|(cur_tok(t).kind == TOK_DIV)|(cur_tok(t).kind == TOK_REM)){
    op_kind_t op = parse_multi_oper(t);
    expr_t e0 = parse_unary_expr(t);
    e = mk_expr_bin_op(filename, line, op, e, e0);
  }
  return e;
}

op_kind_t parse_add_oper(tokenizer_t t){
  if(cur_tok(t).kind == TOK_PLUS){
    eat_it(t, TOK_PLUS);
    return op_kind_bin_plus;
  }
  else if(cur_tok(t).kind == TOK_MINUS){
    eat_it(t, TOK_MINUS);
    return op_kind_bin_minus;
  }
  else{
    printf("exit_3\n");
    exit(1);
  }
}

op_kind_t parse_multi_oper(tokenizer_t t){
  if(cur_tok(t).kind == TOK_MUL){
    eat_it(t, TOK_MUL);
    return op_kind_mult;
  }
  else if(cur_tok(t).kind == TOK_DIV){
    eat_it(t, TOK_DIV);
    return op_kind_div;
  }
  else if(cur_tok(t).kind == TOK_REM){
    eat_it(t, TOK_REM);
    return op_kind_rem;
  }
  else{
    printf("exit_4\n");
    exit(1);
  }
}

expr_t parse_unary_expr(tokenizer_t t){
  char * filename = cur_tok(t).filename;
  int line = cur_tok(t).line_num;
  char * f =(char *)safe_malloc(sizeof(char) * 50) ;
  if(cur_tok(t).kind == TOK_INT_LITERAL){
    strcpy(f, cur_tok(t).num);
    f = (char *)realloc(f, sizeof(char) * strlen(f) + 1);
    eat_it(t, TOK_INT_LITERAL);
    return mk_expr_int_literal(filename, line, f);
  }
  else if(cur_tok(t).kind == TOK_ID){
    strcpy(f, parse_identifier(t));
    f = (char *)realloc(f, sizeof(char) * strlen(f) + 1);
    if(cur_tok(t).kind == TOK_LPAREN){
      eat_it(t, TOK_LPAREN);
      expr_list_t args = parse_arg_expr_list(t);
      eat_it(t, TOK_RPAREN);
      return mk_expr_call(filename, line, f, args);
    }
    else{
      return mk_expr_id(filename, line, f);
    }
  }
  else if(cur_tok(t).kind == TOK_LPAREN){
    eat_it(t, TOK_LPAREN);
    expr_t e = parse_expr(t);
    eat_it(t, TOK_RPAREN);
    return mk_expr_paren(filename, line, e);
  }
  else if((cur_tok(t).kind == TOK_PLUS)|(cur_tok(t).kind == TOK_MINUS)|(cur_tok(t).kind == TOK_BANG)){
    op_kind_t op = parse_unary_oper(t);
    expr_t exp = parse_unary_expr(t);
    return mk_expr_un_op(filename, line, op, exp);
  }
  else{
    printf("exit_5\n");
    exit(1);
  }
}

op_kind_t parse_unary_oper(tokenizer_t t){
  if(cur_tok(t).kind == TOK_PLUS){
    eat_it(t, TOK_PLUS);
    return op_kind_un_plus;
  }
  else if(cur_tok(t).kind == TOK_MINUS){
    eat_it(t, TOK_MINUS);
    return op_kind_un_minus;
  }
  else if(cur_tok(t).kind == TOK_BANG){
    eat_it(t, TOK_BANG);
    return op_kind_logneg;
  }
  else{
    printf("exit_6\n");
    exit(1);
  }
}

expr_list_t parse_arg_expr_list(tokenizer_t t){
  expr_list_t l = mk_expr_list();
  expr_t x;
  if(cur_tok(t).kind != TOK_RPAREN){
   x = parse_expr(t);
   expr_list_add(l, x);
    while(cur_tok(t).kind != TOK_RPAREN){
      eat_it(t, TOK_COMMA);
      x = parse_expr(t);
      expr_list_add(l, x);
    }
  }
  return l;
}

