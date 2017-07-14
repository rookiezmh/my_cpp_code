#include "exit_caller.h"
#include <stdio.h>

void test(void *arg) {
  int *p = (int *) arg;
  ++*p;
}

int main() {
  int i = 0;
  {
    util::ExitCaller(NULL, &i);
  }
  printf("i: %d\n", i);
}
