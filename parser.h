#pragma once

#include "tokenizer.h"
#include "syntree.h"

void eat_it(tokenizer_t t, token_kind_t kind);
program_t parse_program(tokenizer_t t);
fun_def_t parse_fun_difinition(tokenizer_t t);
char * parse_identifier(tokenizer_t t);
void parse_type_expr(tokenizer_t t);
var_decl_list_t parse_para_list(tokenizer_t t);
var_decl_t parse_para(tokenizer_t t);
stmt_t parse_comp_stmt(tokenizer_t t);
var_decl_list_t parse_var_decl(tokenizer_t t);
stmt_t parse_stmt(tokenizer_t t);
stmt_t parse_stmt_if(tokenizer_t t);
stmt_t parse_stmt_while(tokenizer_t t);
expr_t parse_expr(tokenizer_t t);
expr_t parse_equal_expr(tokenizer_t t);
expr_t parse_relat_expr(tokenizer_t t);
op_kind_t parse_equal_oper(tokenizer_t t);
expr_t parse_add_expr(tokenizer_t t);
op_kind_t parse_relat_oper(tokenizer_t t);
expr_t parse_multi_expr(tokenizer_t t);
op_kind_t parse_add_oper(tokenizer_t t);
op_kind_t parse_multi_oper(tokenizer_t t);
expr_t parse_unary_expr(tokenizer_t t);
op_kind_t parse_unary_oper(tokenizer_t t);
expr_list_t parse_arg_expr_list(tokenizer_t t);



