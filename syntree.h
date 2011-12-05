/* 
 * syntree.h
 */
#pragma once

#include <stdio.h>
#include <stdlib.h>

typedef struct list * list_t;

list_t mk_list();
void list_add(list_t l, void * x);
void * list_get(list_t l, int i);
int list_sz(list_t l);
/* 
 * ----------------------------------
 * 式のデータ型定義
 * -----------------------------------
 */

/* 式を表すデータ型 expr_t.

   以下は, expr_tは(のちに定義される)
   struct expr (構造体)へのポインタです, という意味.  */
typedef struct expr * expr_t;

/* 式のリスト expr_list_t */
typedef struct expr_list * expr_list_t;

/* 式の種類 */
typedef enum {
  expr_kind_int_literal,	/* 整数リテラル (0, 1, 2, ...) */
  expr_kind_id,			/* 変数 (x, y, foo, bar, ...) */
  expr_kind_paren,		/* 括弧 ( 式 ) */
  expr_kind_app,		/* 関数呼び出し(f(1, 2)など. 
				   ただし 1 + 2 などもこの一種と見なす) */
} expr_kind_t;

/* 演算子の種類 */
typedef enum {
  op_kind_none, 		/*  */
  op_kind_fun,			/* 組み込み演算子ではない, 関数 */
  op_kind_assign,		/* a = b */
  op_kind_eq,			/* a == b */
  op_kind_neq,			/* a != b */
  op_kind_lt,			/* a < b */
  op_kind_gt,			/* a > b */
  op_kind_le,			/* a <= b */
  op_kind_ge,			/* a >= b */
  op_kind_bin_plus,		/* a + b */
  op_kind_bin_minus,		/* a - b */
  op_kind_mult,			/* a * b */
  op_kind_div,			/* a / b */
  op_kind_rem,			/* a %  b */
  op_kind_un_plus,		/* +a 単項+ */
  op_kind_un_minus,		/* -a 単項- */
  op_kind_logneg,		/* !a */
} op_kind_t;

typedef enum {
  place_redi,
  place_mem,
} info_type_t;

/* コード生成に使われる任意の情報へのポインタ. 今は気にしなくてよい. */
typedef struct syntree_info * syntree_info_t;

struct syntree_info
{
  info_type_t kind;   /*場所の種類*/
  int num;
  char * a;   /*格納場所*/
};

/* 式を表す構造体 expr.  */
struct expr
{
  char * filename;		/* 出現ファイル名 */
  int line;			/* 出現行番号 */
  expr_kind_t kind;		/* 種類 */
  syntree_info_t info;		/* 格納場所に関する情報 */
  /* u : 各種類に応じて必要な情報 */
  union {
    /* 整数リテラル(expr_kind_int_literal)
       または変数(expr_kind_id)の場合. */
    char * s;			/* その文字列表現 */
    /* 括弧の場合(expr_kind_paren) */
    expr_t p;			/* 括弧内の式 */
    /* 関数呼び出し(f(x,y)など)または演算式(x + yなど)の場合. */
    struct app_expr {  
      char * f;	   /* 関数名. 組み込み演算子(+など)の場合はNULL */
      op_kind_t o; /* 演算子の種類. 関数呼び出しの場合はop_kind_fun */
      expr_list_t args;		/* 引数. 式のリスト */
    } a;
  } u;
};

/* -------------------------
 * 変数宣言. 
 * 複合文 { ... } の先頭に現れる, int x; のような.
 * -------------------------
 */

typedef struct var_decl
{
  char * filename;		/* 出現ファイル名 */
  int line;			/* 出現行番号 */
  char * v;			/* 変数名 */
} * var_decl_t;

/* 変数宣言のリスト */
typedef struct var_decl_list * var_decl_list_t;

/* 
 * 文
 *
 * ---------------------------------
 * statement:
 *  ;
 *  continue ;
 *  break ;
 *  return expression ;
 *  expression ;
 *  compound-statement
 *  if-statement
 *  while-statement
 * ---------------------------------
 */

/* 文を表すデータ型 stmt_t.

   以下は, stmt_tは(のちに定義される)
   struct stmt (構造体)へのポインタです, という意味.  */
typedef struct stmt * stmt_t;
typedef struct stmt_list * stmt_list_t;

/* 注: 

   以下の「式文」とは, 式の後ろに; をつけてそれを文とするもの.
   例えば,
   exit(1);
   が文であるのは, exit(1) が関数呼び出しで, それが「式」であるから.
   x = 1;
   も同様で, まず x = 1 が式であるため, x = 1; が文となる.

   複合文とは, { ... } で囲まれたブロックの事.
   例えば if (x < y) { ... } のような.
   実際にはif文のthen部分は任意の文であり,
   複合文が文の一種であるということ.
*/

typedef enum {
  stmt_kind_empty,		/* 空文 ; */
  stmt_kind_continue,		/* continue ; */
  stmt_kind_break,		/* break ; */
  stmt_kind_return,		/* return 式 ; */
  stmt_kind_expr,		/* 式文 ; (f(x, y); など) */
  stmt_kind_compound,		/* 複合文 { ... } */
  stmt_kind_if,			/* if文 */
  stmt_kind_while,		/* while文 */
} stmt_kind_t;

/* 文を表す構造体 */
struct stmt
{
  char * filename;		/* 出現ファイル名 */
  int line;			/* 出現行番号 */
  stmt_kind_t kind;		/* 文の種類 */
  syntree_info_t info;
  /* u : 文の種類に応じた情報 */
  union {
    /* 式文, return文の場合. */
    expr_t e;
    /* 複合文の場合 */
    struct {
      var_decl_list_t decls;	/* 変数宣言のリスト */
      stmt_list_t body;		/* 本体の文のリスト */
    } c;
    /* if文の場合: if (e) th else el */
    struct {
      expr_t e;			/* 条件式 */
      stmt_t th;		/* thenの文 */
      stmt_t el;		/* elseの文. ない場合はNULL */
    } i;
    /* while文の場合: while (e) body */
    struct {
      expr_t e;			/* 条件式 */
      stmt_t body;		/* 繰り返し本体 */
    } w;
  } u;
};

/* 
 * 関数定義
 * ---------------------------------
 * function-definition:
 *    type-expression identifier ( parameter-list ) compound-statement
 *
 * e.g., 
 *    int f(int x, int y, int z) S
 * --------------------------------- 
 */

typedef struct fun_def
{
  char * filename;		/* 出現ファイル名 */
  int line;			/* 出現行番号 */
  char * f;			/* 定義される関数名 (f) */
  var_decl_list_t params;	/* 引数リスト (int x, int y, int z) */
  stmt_t body;			/* 本体 (S). 実際は必ず複合文 */
} * fun_def_t;

/* 関数定義のリスト */
typedef struct fun_def_list * fun_def_list_t;

/* 
 * プログラム = 1ファイル中の内容全体
 * ---------------------------------
 * program:
 *   definition* eof
 * --------------------------------- 
 */

typedef struct program {
  char * filename;		/* ファイル名 */
  fun_def_list_t fun_defs;	/* 関数定義のリスト */
} * program_t;


/* 
 * -----------------------------------------------
 * インタフェース関数群 
 * -----------------------------------------------
 */

/* 式のコンストラクタ */
expr_t mk_expr_int_literal(char * filename, int line, char * i);
expr_t mk_expr_id(char * filename, int line, char * v);
expr_t mk_expr_paren(char * filename, int line, expr_t e);
expr_t mk_expr_call(char * filename, int line, char * f, expr_list_t args);
expr_t mk_expr_bin_op(char * filename, int line, op_kind_t op, expr_t e0, expr_t e1);
expr_t mk_expr_un_op(char * filename, int line, op_kind_t op, expr_t e);

/* opが単項演算子なら演算子を文字列で返す */
char * un_op_to_str(op_kind_t op);
/* opが2項演算子なら演算子を文字列で返す */
char * bin_op_to_str(op_kind_t op);

/* 式のリスト関係の関数 */
expr_list_t mk_expr_list();	/* 空のリストを作るコンストラクタ */
void expr_list_add(expr_list_t l, expr_t x); /* lにxを追加 */
expr_t expr_list_get(expr_list_t l, int i);  /* lのi番目の要素 */
int expr_list_sz(expr_list_t l);	     /* lの要素数 */

/* 変数宣言のコンストラクタ */
var_decl_t mk_var_decl(char * filename, int line, char * v);

/* 変数宣言のリスト関係の関数. 意味は式のリスト(expr_list_t)のそれと同じ. */
var_decl_list_t mk_var_decl_list();
void var_decl_list_add(var_decl_list_t l, var_decl_t x);
var_decl_t var_decl_list_get(var_decl_list_t l, int i);
int var_decl_list_sz(var_decl_list_t l);

/* 文のコンストラクタ */
stmt_t mk_stmt_empty(char * filename, int line);
stmt_t mk_stmt_continue(char * filename, int line);
stmt_t mk_stmt_break(char * filename, int line);
stmt_t mk_stmt_return(char * filename, int line, expr_t e);
stmt_t mk_stmt_expr(char * filename, int line, expr_t e);
stmt_t mk_stmt_compound(char * filename, int line, var_decl_list_t d, stmt_list_t c);
stmt_t mk_stmt_if(char * filename, int line, expr_t e, stmt_t th, stmt_t el);
stmt_t mk_stmt_while(char * filename, int line, expr_t e, stmt_t body);

/* 文のリスト関係の関数. 意味は式のリスト(expr_list_t)のそれと同じ. */
stmt_list_t mk_stmt_list();
void stmt_list_add(stmt_list_t l, stmt_t x);
stmt_t stmt_list_get(stmt_list_t l, int i);
int stmt_list_sz(stmt_list_t l);

/* 関数定義のコンストラクタ */
fun_def_t mk_fun_def(char * filename, int line, 
		     char * f, var_decl_list_t params, stmt_t body);

/* 関数定義のリスト関係の関数. 意味は式のリスト(expr_list_t)のそれと同じ. */
fun_def_list_t mk_fun_def_list();
void fun_def_list_add(fun_def_list_t l, fun_def_t x);
fun_def_t fun_def_list_get(fun_def_list_t l, int i);
int fun_def_list_sz(fun_def_list_t l);

/* プログラムのコンストラクタ */
program_t mk_program(char * filename, fun_def_list_t ds);


/* 式, 文, プログラムを表示する関数群 */
void pr_expr(FILE * fp, expr_t e);
void pr_stmt(FILE * fp, stmt_t s);
void pr_program(FILE * fp, program_t p);

FILE * safe_fopen(char * filename, char * mode);
void * safe_malloc(ssize_t sz);
char * safe_strdup(char *);



