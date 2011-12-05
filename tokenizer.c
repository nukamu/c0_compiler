#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "parser.h"
#include "syntree.h"
#include "tokenizer.h"

int flag = 1;

tokenizer_t mk_tokenizer(char * filename){
  tokenizer_t t = (tokenizer_t)malloc(sizeof(struct tokenizer));
  FILE * fp = safe_fopen(filename, "r");
  t->tok.filename = (char *)malloc((strlen(filename)+1)*sizeof(char));
  int c;
  t->tok.a = (char *)malloc(sizeof(char) * 100);
  t->tok.num = (char *)malloc(sizeof(char) * 100);
  c = fgetc(fp);
  t->c = c;
  t->fp = fp;
  t->tok.filename = filename;
  t->tok.line_num = 1;
  next_tok(t);
  return t;
}

token cur_tok(tokenizer_t t){
  return t->tok;
}

token next_tok(tokenizer_t t){
  if(flag == 0){
    t->c = fgetc(t->fp);
  }
  flag = 0;
  int i = 0;
  char a[100];
  while((t->c == ' ')|(t->c == '\n')|(t->c == '\t')){
    if(t->c == '\n'){
      t->tok.line_num++;
    }
    t->c = fgetc(t->fp);
  }
  switch(t->c){
  case EOF:
    t->tok.kind = TOK_EOF;
    break;
  case '(':
    t->tok.kind = TOK_LPAREN;
    break;
  case ')':
    t->tok.kind = TOK_RPAREN;
    break;
  case '{':
    t->tok.kind = TOK_LBRACE;
    break;
  case '}':
    t->tok.kind = TOK_RBRACE;
    break;
  case '*':
    t->tok.kind = TOK_MUL;
    break;
  case '+':
    t->tok.kind = TOK_PLUS;
    break;
  case '-':
    t->tok.kind = TOK_MINUS;
    break;
  case '/':
    t->tok.kind = TOK_DIV;
    break;
  case '%':
    t->tok.kind = TOK_REM;
    break;
  case ';':
    t->tok.kind = TOK_SEMICOLON;
    break;
  case ',':
    t->tok.kind = TOK_COMMA;
    break;
  default:
    t->tok.kind = TOK_RIDDLE;
    break;
  }
  if(t->c == '<'){
    t->c = fgetc(t->fp);
    if(t->c == '='){
      t->tok.kind = TOK_LE;
    }
    else{
      t->tok.kind = TOK_LT;
      flag = 1;
    }
  }
  else if(t->c == '>'){
    t->c = fgetc(t->fp);
    if(t->c == '='){
      t->tok.kind = TOK_GE;
    }
    else{
      t->tok.kind = TOK_GT;
      flag = 1;
    }
  }
  else if(t->c == '!'){
    t->c = fgetc(t->fp);
    if(t->c == '='){
      t->tok.kind = TOK_NEQ;
    }
    else{
      t->tok.kind = TOK_BANG;
      flag = 1;
    }
  }
  else if(t->c == '='){
    t->c = fgetc(t->fp);
    if(t->c == '='){
      t->tok.kind = TOK_EQ;
    }
    else{
      t->tok.kind = TOK_ASSIGN;
      flag = 1;
    }
  } 
  else if((isalpha(t->c))|(t->c == '_')){
    while((isalpha(t->c)) | (isdigit(t->c)) | (t->c =='_')){
      a[i++] = t->c;
      t->c = fgetc(t->fp);
    }
    a[i] = '\0';
    if(strcmp(a, "break") == 0){
      t->tok.kind = TOK_BREAK;
    }
    else if(strcmp(a, "continue") == 0){
      t->tok.kind = TOK_CONTINUE;
    }
    else if(strcmp(a, "int") == 0){
      t->tok.kind = TOK_INT;
    }
    else if(strcmp(a, "else") == 0){
      t->tok.kind = TOK_ELSE;
    }
    else if(strcmp(a, "if") == 0){
      t->tok.kind = TOK_IF;
    }
    else if(strcmp(a, "return") == 0){
      t->tok.kind = TOK_RETURN;
    }
    else if(strcmp(a, "while") == 0){
      t->tok.kind = TOK_WHILE;
    }
    else{
      t->tok.kind = TOK_ID;
      strcpy(t->tok.a, a);
    }
    flag = 1;
  }
  else if(isdigit(t->c)){
    t->tok.kind = TOK_INT_LITERAL;
    while(isdigit(t->c)){
      a[i++] = t->c;
      t->c = fgetc(t->fp);
    }
    a[i] = '\0';

    strcpy(t->tok.num , a);
    flag = 1;
  }
  return t->tok;
}
