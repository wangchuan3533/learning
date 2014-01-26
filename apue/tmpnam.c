#include <stdio.h>
#include <stdlib.h>
int main()
{
  char *name = tmpnam(NULL);
  FILE *fp = tmpfile();
  printf("%s\n", name);
  fprintf(fp, "hello");

  exit(0);
}
