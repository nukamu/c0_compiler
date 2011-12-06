#include <stdio.h>
#include "cogen.h"
#include "tokenizer.h"
#include "syntree.h"
#include "parser.h"

int main(int argc, char ** argv){
  FILE * fp = stdout;
  if (argc < 2 || argc > 3){
    printf("Usage: %s input_file [output_file]\n", argv[0]);
  }
  tokenizer_t t = mk_tokenizer(argv[1]);
  program_t p = parse_program(t);
  if (argc == 3) fp = safe_fopen(argv[2], "w");
  cogen_program(fp, p);
  return 0;
}
