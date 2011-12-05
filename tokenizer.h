/* 
 * tokenizer.h
 */
#pragma once

typedef enum{
  TOK_INT_LITERAL,  /*number*/
  TOK_ID,   /*variable*/
  TOK_INT,  /*int*/
  TOK_BREAK,   /*break*/
  TOK_CONTINUE,   /*continue*/
  TOK_ELSE,   /*else*/
  TOK_IF,   /*if*/
  TOK_RETURN,    /*return*/
  TOK_WHILE,    /*while*/
  TOK_LPAREN,  /*(*/
  TOK_RPAREN,   /*)*/
  TOK_LBRACE,   /*{*/
  TOK_RBRACE,   /*}*/
  TOK_MUL,    /* * */
  TOK_PLUS,  /*+*/
  TOK_MINUS,  /*-*/
  TOK_DIV,   /*/*/
  TOK_REM,   /*%*/
  TOK_BANG,   /*!*/
  TOK_LT,   /*<*/
  TOK_GT,   /*>*/
  TOK_LE,   /*<=*/
  TOK_GE,   /*>=*/
  TOK_EQ,   /*==*/
  TOK_NEQ,   /*!=*/
  TOK_SEMICOLON,  /*;*/
  TOK_ASSIGN,  /*=*/
  TOK_COMMA,   /*,*/
  TOK_EOF,    /*EOF*/
  TOK_RIDDLE
} token_kind_t;

typedef struct token{
  token_kind_t kind;
  char * num;   /*変数の値*/
  char * a;   /*変数名用*/
  int line_num;
  char *filename;
} token;

typedef struct tokenizer{
  token tok;
  int c;
  FILE *fp;
} * tokenizer_t;

tokenizer_t mk_tokenizer(char * filename);
token cur_tok(tokenizer_t t);
token next_tok(tokenizer_t t);
