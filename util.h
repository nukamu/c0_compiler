/* 
 * util.h
 */
#pragma once

#include <stdio.h>
#include <stdlib.h>

/* fopen/malloc/strdup + 成功確認.
   これらを通してしか, それらを呼ばない */
FILE * safe_fopen(char * filename, char * mode);
void * safe_malloc(ssize_t sz);
char * safe_strdup(char *);

