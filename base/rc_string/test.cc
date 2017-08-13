#include "rc_string.h"
#include <stdio.h>

using util::String;

int main() {
  String s("hello");
  printf("%s", s.c_str());
}
