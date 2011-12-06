#include "tokenizer.h"

/**
 * This function is called only once when make tokenizer.
 * 
 * @param filename name of parsed file
 * @return tokenizer we can use
 */
tokenizer_t mk_tokenizer(char * filename){
  tokenizer_t t = (tokenizer_t)malloc(sizeof(struct tokenizer));

  /* open parsed file here */
  FILE * fp = safe_fopen(filename, "rb");
  t->tok.a = (char *)malloc(sizeof(char) * 1024);
  t->tok.num = (char *)malloc(sizeof(char) * 1024);
  /*TODO: should remove limit of the number of chars*/


  /* initialize each element */
  t->c = fgetc(fp);
  t->fp = fp;
  t->tok.filename = filename;
  t->tok.line_num = 1;
  next_tok(t);
  return t;
}

/**
 * This function returns current token.
 *
 * @param t used toknizer
 * @return current token
 */
token cur_tok(tokenizer_t t){
  return t->tok;
}

/**
 * This function returns next token.
 *
 * @param t used tokenizer
 * @return next token
 */
token next_tok(tokenizer_t t){
  int i = 0; // counter of int-literal and identifier
  char a[1024];  // temp buffer of strings
  /*TODO: should remove limit of the number of chars*/

  /* initialize each elements of token */
  t->tok.kind = TOK_RIDDLE;
  strcpy(t->tok.num, "");
  strcpy(t->tok.a, "");

  /* skip ' ',  '\n' and '\t' */
  while((t->c == ' ')|(t->c == '\n')|(t->c == '\t')){
    if(t->c == '\n'){
      t->tok.line_num++;
    }
    t->c = fgetc(t->fp);
  }

  switch(t->c){
  case EOF:
    t->tok.kind = TOK_EOF;
    t->c = fgetc(t->fp);
    break;
  case '(':
    t->tok.kind = TOK_LPAREN;
    t->c = fgetc(t->fp);
    break;
  case ')':
    t->tok.kind = TOK_RPAREN;
    t->c = fgetc(t->fp);
    break;
  case '{':
    t->tok.kind = TOK_LBRACE;
    t->c = fgetc(t->fp);
    break;
  case '}':
    t->tok.kind = TOK_RBRACE;
    t->c = fgetc(t->fp);
    break;
  case '*':
    t->tok.kind = TOK_MUL;
    t->c = fgetc(t->fp);
    break;
  case '+':
    t->tok.kind = TOK_PLUS;
    t->c = fgetc(t->fp);
    break;
  case '-':
    t->tok.kind = TOK_MINUS;
    t->c = fgetc(t->fp);
    break;
  case '/':
    t->tok.kind = TOK_DIV;
    t->c = fgetc(t->fp);
    break;
  case '%':
    t->tok.kind = TOK_REM;
    t->c = fgetc(t->fp);
    break;
  case ';':
    t->tok.kind = TOK_SEMICOLON;
    t->c = fgetc(t->fp);
    break;
  case ',':
    t->tok.kind = TOK_COMMA;
    t->c = fgetc(t->fp);
    break;
  default:
    t->tok.kind = TOK_RIDDLE;
    break;
  }
  if(t->c == '<'){
    t->c = fgetc(t->fp); // read one more char
    if(t->c == '='){ // case of "<="
      t->tok.kind = TOK_LE;
      t->c = fgetc(t->fp);
    }
    else{ // case of "<"
      t->tok.kind = TOK_LT;
    }
  }
  else if(t->c == '>'){
    t->c = fgetc(t->fp); // read one more char
    if(t->c == '='){ // case of ">="
      t->tok.kind = TOK_GE;
      t->c = fgetc(t->fp);
    }
    else{ // case of ">"
      t->tok.kind = TOK_GT;
    }
  }
  else if(t->c == '!'){
    t->c = fgetc(t->fp); // read one more char
    if(t->c == '='){ // case of "!="
      t->tok.kind = TOK_NEQ;
    }
    else{ // case of "!"
      t->tok.kind = TOK_BANG;
    }
  }
  else if(t->c == '='){
    t->c = fgetc(t->fp); // read one more char
    if(t->c == '='){ // case of "=="
      t->tok.kind = TOK_EQ;
      t->c = fgetc(t->fp);
    }
    else{ // case of "="
      t->tok.kind = TOK_ASSIGN;
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
  }
  else if(isdigit(t->c)){
    t->tok.kind = TOK_INT_LITERAL;
    while(isdigit(t->c)){
      a[i++] = t->c;
      t->c = fgetc(t->fp);
    }
    a[i] = '\0';
    strcpy(t->tok.num , a);
  }
  return t->tok;
}

void print_tok_kind(tokenizer_t t){
  token tok = cur_tok(t);
  int line_num = tok.line_num;
  switch(tok.kind){
  case TOK_INT_LITERAL:
    printf("%d:TOK_INT_LITERAL (%s)\n", line_num, t->tok.num);
    break;
  case TOK_ID:
    printf("%d:TOK_ID (%s)\n", line_num, t->tok.a);
    break;
  case TOK_INT:
    printf("%d:TOK_INT\n", line_num);
    break;
  case TOK_BREAK:
    printf("%d:TOK_BREAK\n", line_num);
    break;
  case TOK_CONTINUE:
    printf("%d:TOK_CONTINUE\n", line_num);
    break;
  case TOK_ELSE:
    printf("%d:TOK_ELSE\n", line_num);
    break;
  case TOK_IF:
    printf("%d:TOK_IF\n", line_num);
    break;
  case TOK_RETURN:
    printf("%d:TOK_RETURN\n", line_num);
    break;
  case TOK_WHILE:
    printf("%d:TOK_WHILE\n", line_num);
    break;
  case TOK_LPAREN:
    printf("%d:TOK_LPAREN\n", line_num);
    break;
  case TOK_RPAREN:
    printf("%d:TOK_RPAREN\n", line_num);
    break;
  case TOK_LBRACE:
    printf("%d:TOK_LBRACE\n", line_num);
    break;
  case TOK_RBRACE:
    printf("%d:TOK_RBRACE\n", line_num);
    break;
  case TOK_MUL:
    printf("%d:TOK_MUL\n", line_num);
    break;
  case TOK_MINUS:
    printf("%d:TOK_MINUS\n", line_num);
    break;
  case TOK_PLUS:
    printf("%d:TOK_PLUS\n", line_num);
    break;
  case TOK_DIV:
    printf("%d:TOK_DIV\n", line_num);
    break;
  case TOK_REM:
    printf("%d:TOK_REM\n", line_num);
    break;
  case TOK_BANG:
    printf("%d:TOK_BANG\n", line_num);
    break;
  case TOK_LT:
    printf("%d:TOK_LT\n", line_num);
    break;
  case TOK_GT:
    printf("%d:TOK_GT\n", line_num);
    break;
  case TOK_LE:
    printf("%d:TOK_LE\n", line_num);
    break;
  case TOK_GE:
    printf("%d:TOK_GE\n", line_num);
    break;
  case TOK_EQ:
    printf("%d:TOK_EQ\n", line_num);
    break;
  case TOK_NEQ:
    printf("%d:TOK_NEQ\n", line_num);
    break;
  case TOK_SEMICOLON:
    printf("%d:TOK_SEMICOLON\n", line_num);
    break;
  case TOK_ASSIGN:
    printf("%d:TOK_ASSIGN\n", line_num);
    break;
  case TOK_COMMA:
    printf("%d:TOK_COMMA\n", line_num);
    break;
  case TOK_EOF:
    break;
  default:
    printf("** error: This is unexpected token **\n");
    break;
  }
}

