/* 
 * util.c
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"

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
