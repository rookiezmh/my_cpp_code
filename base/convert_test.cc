#include "convert.h"
#include <stdio.h>

int main() {
  char buf[128];
  long i = 1231231123123;
  //Convert(buf, i);
  sprintf(buf, "%ld", i);
  printf("%s\n", buf);
}
