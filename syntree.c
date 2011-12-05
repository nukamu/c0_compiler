/* 
 * syntree.c
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "syntree.h"


struct list {
  /* invariant:
     a has sz elements
     n <= sz
     a[sz - 1] is accessible. */
  void ** a;			/* 要素(void*)の配列 */
  int n;			/* 埋まっている要素数 */
  int sz;			/* aに格納可能な要素数 */
};

/* 空のリストを作る */
list_t mk_list()
{
  int sz = 1;
  list_t b = (list_t)safe_malloc(sizeof(struct list));
  b->a = (void **)safe_malloc(sz * sizeof(void*));
  b->n = 0;
  b->sz = sz;
  return b;
}

/* 補助関数. b->a が n 要素持つように,
   必要ならば配列を取り直して保証する. */
static void list_ensure_sz(list_t b, int n)
{
  int sz = b->sz;
  if (n >= sz) {
    int new_sz = sz * 2;
    if (new_sz <= sz) new_sz = sz + 1;
    void ** new_a = (void **)safe_malloc(new_sz * sizeof(void *));
    memcpy(new_a, b->a, sz * sizeof(void *));
    b->a = new_a;
    b->sz = new_sz;
  }
}

/* bの後ろにcを加える */
void list_add(list_t b, void * c)
{
  list_ensure_sz(b, b->n + 1);
  int n = b->n;
  assert(n < b->sz);
  b->a[n] = c;
  b->a[n + 1] = 0;
  b->n = n + 1;
}

/* bの第i要素を返す */
void * list_get(list_t b, int i)
{
  assert(i < b->n);
  return b->a[i];
}

/* bに格納されている要素数 */
int list_sz(list_t b)
{
  return b->n;
}

/* fopen + 成功確認 */
FILE * safe_fopen(char * filename, char * mode)
{
  FILE * fp = fopen(filename, mode);
  if (fp == NULL) {
    perror("fopen");
    exit(1);
  }
  return fp;
}

/* malloc + 成功確認 */
void * safe_malloc(ssize_t sz)
{
  void * a = malloc(sz);
  if (a == NULL) {
    perror("malloc");
    exit(1);
  }
  return a;
}

/* strdup + 成功確認 */
char * safe_strdup(char * s)
{
  int n = strlen(s);
  char * a = (char *)safe_malloc(n + 1);
  int i;
  for (i = 0; i < n + 1; i++) a[i] = s[i];
  return a;
}
/* 
 * ----------------------------------
 * 式のコンストラクタ達
 * -----------------------------------
 */

/* 
 * 補助関数 : 種類がkindの式ノードを作る. filename, lineはそれが出現した
 *          ファイル名と行番号(エラーメッセージを表示するために保存する)
 */
static expr_t alloc_expr(char * filename, int line, expr_kind_t kind)
{
  expr_t e = (expr_t)safe_malloc(sizeof(struct expr));
  e->filename = filename;
  e->line = line;
  e->kind = kind;
  e->info = NULL;
  return e;
}

/* 整数リテラル (0, 1, 2, ...) の式ノードを作る */
expr_t mk_expr_int_literal(char * filename, int line, char * i)
{
  expr_t e = alloc_expr(filename, line, expr_kind_int_literal);
  e->u.s = i;
  return e;
}

/* 変数 (x, y, foo, bar, ...) の式ノードを作る */
expr_t mk_expr_id(char * filename, int line, char * v)
{
  expr_t e = alloc_expr(filename, line, expr_kind_id);
  e->u.s = v;
  return e;
}

/* 括弧 ( 式 ) の式ノードを作る */
expr_t mk_expr_paren(char * filename, int line, expr_t p)
{
  expr_t e = alloc_expr(filename, line, expr_kind_paren);
  e->u.p = p;
  return e;
}

/* 関数呼び出し(f(式, 式, ...))の式ノードを作る 
   f : 関数名. args : 引数のリスト */
expr_t mk_expr_call(char * filename, int line, char * f, expr_list_t args)
{
  expr_t e = alloc_expr(filename, line, expr_kind_app);
  e->u.a.o = op_kind_fun;
  e->u.a.f = f;
  e->u.a.args = args;
  return e;
}

/* 2項演算式の式ノードを作る  */
expr_t mk_expr_bin_op(char * filename, int line, op_kind_t op, expr_t e0, expr_t e1)
{
  expr_t e = alloc_expr(filename, line, expr_kind_app);
  expr_list_t args = mk_expr_list();
  expr_list_add(args, e0);
  expr_list_add(args, e1);
  e->u.a.o = op;
  e->u.a.f = NULL;
  e->u.a.args = args;
  return e;
}

/* 単項演算式の式ノードを作る  */
expr_t mk_expr_un_op(char * filename, int line, op_kind_t op, expr_t a)
{
  expr_t e = alloc_expr(filename, line, expr_kind_app);
  expr_list_t args = mk_expr_list();
  expr_list_add(args, a);
  e->u.a.o = op;
  e->u.a.f = NULL;
  e->u.a.args = args;
  return e;
}

/* 式のリスト関係の関数.
   中身は実際には作らずに, list (void *のリスト)
   の関数を呼び出すだけ. */

/* 式のリスト(空)を作る */
expr_list_t mk_expr_list()
{
  return (expr_list_t)mk_list();
}

/* 式のリストに式を追加する */
void expr_list_add(expr_list_t l, expr_t x)
{
  list_add((list_t)l, (void *)x);
}

/* 式のリストのi番目の要素を返す */
expr_t expr_list_get(expr_list_t l, int i)
{
  return (expr_t)list_get((list_t)l, i);
}

/* 式のリストの長さを返す */
int expr_list_sz(expr_list_t l)
{
  return list_sz((list_t)l);
}

/* -------------------------
 * 変数宣言  int z; に相当する構文木のノード
 * intしかないので,
   実質的な情報は変数名だけ 
 * -------------------------
 */

/* コンストラクタ. vは定義される変数名 */
var_decl_t mk_var_decl(char * filename, int line, char * v)
{
  var_decl_t d = (var_decl_t)safe_malloc(sizeof(struct var_decl));
  d->v = v;
  return d;
}

/* 変数宣言のリスト関係の関数. 意味は expr_listの場合と同じ */
var_decl_list_t mk_var_decl_list()
{
  return (var_decl_list_t)mk_list();
}

void var_decl_list_add(var_decl_list_t l, var_decl_t x)
{
  list_add((list_t)l, (void *)x);
}

var_decl_t var_decl_list_get(var_decl_list_t l, int i)
{
  return (var_decl_t)list_get((list_t)l, i);
}

int var_decl_list_sz(var_decl_list_t l)
{
  return list_sz((list_t)l);
}

/* 
 * ---------------------------------
 * 文
 * ---------------------------------
 */

/* 
 * 補助関数 : 種類がkindの文ノードを作る. filename, lineはそれが出現した
 *          ファイル名と行番号(エラーメッセージを表示するために保存する)
 */
static stmt_t alloc_stmt(char * filename, int line, stmt_kind_t kind)
{
  stmt_t s = (stmt_t)safe_malloc(sizeof(struct stmt));
  s->filename = filename;
  s->line = line;
  s->kind = kind;
  s->info = NULL;
  return s;
}

/* 空文の文ノードを作る */
stmt_t mk_stmt_empty(char * filename, int line)
{
  return alloc_stmt(filename, line, stmt_kind_empty);
}

/* continue文の文ノードを作る */
stmt_t mk_stmt_continue(char * filename, int line)
{
  return alloc_stmt(filename, line, stmt_kind_continue);
}

/* break文の文ノードを作る */
stmt_t mk_stmt_break(char * filename, int line)
{
  return alloc_stmt(filename, line, stmt_kind_break);
}

/* return e; の文ノードを作る */
stmt_t mk_stmt_return(char * filename, int line, expr_t e)
{
  stmt_t s = alloc_stmt(filename, line, stmt_kind_return);
  s->u.e = e;
  return s;
}

/* 式文 e; の文ノードを作る */
stmt_t mk_stmt_expr(char * filename, int line, expr_t e)
{
  stmt_t s = alloc_stmt(filename, line, stmt_kind_expr);
  s->u.e = e;
  return s;
}

/* 複合文 の文ノードを作る. decls : 変数宣言. body 本体の文のリスト */
stmt_t mk_stmt_compound(char * filename, int line, 
			var_decl_list_t decls, stmt_list_t body)
{
  stmt_t s = alloc_stmt(filename, line, stmt_kind_compound);
  s->u.c.decls = decls;
  s->u.c.body = body;
  return s;
}

/* if文の文ノードを作る (e) th else el  */
stmt_t mk_stmt_if(char * filename, int line, expr_t e, stmt_t th, stmt_t el)
{
  stmt_t s = alloc_stmt(filename, line, stmt_kind_if);
  s->u.i.e = e;
  s->u.i.th = th;
  s->u.i.el = el;
  return s;
}

/* while文文の文ノードを作る  while(e) body */
stmt_t mk_stmt_while(char * filename, int line, expr_t e, stmt_t body)
{
  stmt_t s = alloc_stmt(filename, line, stmt_kind_while);
  s->u.w.e = e;
  s->u.w.body = body;
  return s;
}

/* 文のリスト関係 */
stmt_list_t mk_stmt_list()
{
  return (stmt_list_t)mk_list();
}

void stmt_list_add(stmt_list_t l, stmt_t x)
{
  list_add((list_t)l, (void *)x);
}

stmt_t stmt_list_get(stmt_list_t l, int i)
{
  return (stmt_t)list_get((list_t)l, i);
}

int stmt_list_sz(stmt_list_t l)
{
  return list_sz((list_t)l);
}

/* ---------------------------------
 * 関数定義
 * --------------------------------- 
 */

/* コンストラクタ */
fun_def_t mk_fun_def(char * filename, int line, 
		     char * f, var_decl_list_t params, stmt_t body)
{
  fun_def_t d = (fun_def_t)safe_malloc(sizeof(struct fun_def));
  d->filename = filename;
  d->line = line;
  d->f = f;
  d->params = params;
  d->body = body;
  return d;
}

/* 関数定義のリスト関係 */
fun_def_list_t mk_fun_def_list()
{
  return (fun_def_list_t)mk_list();
}

void fun_def_list_add(fun_def_list_t l, fun_def_t x)
{
  list_add((list_t)l, (void *)x);
}

fun_def_t fun_def_list_get(fun_def_list_t l, int i)
{
  return (fun_def_t)list_get((list_t)l, i);
}

int fun_def_list_sz(fun_def_list_t l)
{
  return list_sz((list_t)l);
}

/* ---------------------------------
 * 1ファイル中のプログラム全体
 * --------------------------------- 
 */

/* コンストラクタ */
program_t mk_program(char * filename, fun_def_list_t fun_defs)
{
  program_t p = (program_t)safe_malloc(sizeof(struct program));
  p->filename = filename;
  p->fun_defs = fun_defs;
  return p;
}

/* 
 * 構文木を表示する関数群
 */

/* 式のリストを , で区切りながら表示 */
void pr_expr_list_comma(FILE * fp, expr_list_t es)
{
  int n = expr_list_sz(es);
  int i;
  for (i = 0; i < n; i++) {
    if (i > 0) fprintf(fp, ", ");
    expr_t e = expr_list_get(es, i);
    pr_expr(fp, e);
  }
}

/* opが単項演算子なら演算子を文字列で返す */
char * un_op_to_str(op_kind_t op)
{
  switch (op) {
  case op_kind_un_plus: return "+";
  case op_kind_un_minus: return "-";
  case op_kind_logneg:    return "!";
  default:
    return NULL;
  }
}

/* opが2項演算子なら演算子を文字列で返す */
char * bin_op_to_str(op_kind_t op)
{
  switch (op) {
  case op_kind_assign:    return "=";
  case op_kind_eq:        return "==";
  case op_kind_neq:       return "!=";
  case op_kind_lt:        return "<";
  case op_kind_gt:        return ">";
  case op_kind_le:        return "<=";
  case op_kind_ge:        return ">=";
  case op_kind_bin_plus:  return "+";
  case op_kind_bin_minus: return "-";
  case op_kind_mult:      return "*";
  case op_kind_div:       return "/";
  case op_kind_rem:       return "%";
  default:
    return NULL;
  }
}

/* 式を表示 */
void pr_expr(FILE * fp, expr_t e)
{
  switch (e->kind) {
  case expr_kind_int_literal:
  case expr_kind_id:
    fprintf(fp, "%s", e->u.s);
    free(e->u.s);
    break;
  case expr_kind_paren:
    fprintf(fp, "(");
    pr_expr(fp, e->u.p);
    fprintf(fp, ")");
    break;
  case expr_kind_app:
    if (e->u.a.o == op_kind_fun) {
      fprintf(fp, "%s", e->u.a.f);
      free(e->u.a.f);
      fprintf(fp, "(");
      pr_expr_list_comma(fp, e->u.a.args);
      fprintf(fp, ")");
    } else if (un_op_to_str(e->u.a.o)) {
      fprintf(fp, "%s ", un_op_to_str(e->u.a.o));
      pr_expr(fp, expr_list_get(e->u.a.args, 0));
    } else if (bin_op_to_str(e->u.a.o)) {
      pr_expr(fp, expr_list_get(e->u.a.args, 0));
      fprintf(fp, " %s ", bin_op_to_str(e->u.a.o));
      pr_expr(fp, expr_list_get(e->u.a.args, 1));
    } else {
      assert(0);
    }
    break;
  default:
    assert(0);
  }
}

/* 変数宣言を表示 */
void pr_var_decl(FILE * fp, var_decl_t d)
{
  fprintf(fp, "int %s", d->v);
  free(d->v);
}

/* 変数宣言のリストを表示. 各宣言の後ろに ; をつけて */
void pr_var_decl_list_semicolon(FILE * fp, var_decl_list_t ds)
{
  int n = var_decl_list_sz(ds);
  int i;
  for (i = 0; i < n; i++) {
    var_decl_t d = var_decl_list_get(ds, i);
    pr_var_decl(fp, d);
    fprintf(fp, ";\n");
  }
}

/* 変数宣言のリストを表示. 各宣言の後ろに , をつけて */
void pr_var_decl_list_comma(FILE * fp, var_decl_list_t ds)
{
  int n = var_decl_list_sz(ds);
  int i;
  for (i = 0; i < n; i++) {
    if (i > 0) fprintf(fp, ", ");
    var_decl_t d = var_decl_list_get(ds, i);
    pr_var_decl(fp, d);
  }
}

/* 文のリストを表示 */
void pr_stmt_list(FILE * fp, stmt_list_t ss)
{
  int n = stmt_list_sz(ss);
  int i;
  for (i = 0; i < n; i++) {
    stmt_t s = stmt_list_get(ss, i);
    pr_stmt(fp, s);
  }
}

/* 文を表示 */
void pr_stmt(FILE * fp, stmt_t s)
{
  switch (s->kind) {
  case stmt_kind_empty:
    fprintf(fp, ";\n");
    break;
  case stmt_kind_continue:
    fprintf(fp, "continue;\n");
    break;
  case stmt_kind_break:
    fprintf(fp, "break;\n");
    break;
  case stmt_kind_return:
    fprintf(fp, "return ");
    pr_expr(fp, s->u.e);
    fprintf(fp, ";\n");
    break;
  case stmt_kind_expr:
    pr_expr(fp, s->u.e);
    fprintf(fp, ";\n");
    break;
  case stmt_kind_compound:
    fprintf(fp, "{\n");
    pr_var_decl_list_semicolon(fp, s->u.c.decls);
    pr_stmt_list(fp, s->u.c.body);
    fprintf(fp, "}\n");
    break;
  case stmt_kind_if:
    fprintf(fp, "if (");
    pr_expr(fp, s->u.i.e);
    fprintf(fp, ") ");
    pr_stmt(fp, s->u.i.th);
    if (s->u.i.el) {
      fprintf(fp, " else ");
      pr_stmt(fp, s->u.i.el);
    }
    break;
  case stmt_kind_while:
    fprintf(fp, "while(");
    pr_expr(fp, s->u.w.e);
    fprintf(fp, ") ");
    pr_stmt(fp, s->u.w.body);
    break;
  default:
    assert(0);
  }
}

/* 関数定義を表示 */
void pr_fun_def(FILE * fp, fun_def_t d)
{
  fprintf(fp, "int %s(", d->f);
  free(d->f);
  pr_var_decl_list_comma(fp, d->params);
  fprintf(fp, ") ");
  pr_stmt(fp, d->body);
}

/* 関数定義のリストを表示 */
void pr_fun_def_list(FILE * fp, fun_def_list_t ds)
{
  int n = fun_def_list_sz(ds);
  int i;
  for (i = 0; i < n; i++) {
    if (i > 0) fprintf(fp, "\n");
    fun_def_t d = fun_def_list_get(ds, i);
    pr_fun_def(fp, d);
  }
}

/* プログラム全体(関数定義のリスト)を表示 */
void pr_program(FILE * fp, program_t p)
{
  pr_fun_def_list(fp, p->fun_defs);
  fprintf(fp, "\n");
}



