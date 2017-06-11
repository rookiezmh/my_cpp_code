#include "dynamic_words_list.h"
#include <pthread.h>
#include <string>

using namespace std;

string Test(void *arg) {
  return "./dynamic_words_list.h";
}

int main() {
  util::DynamicWordsList wl;
  wl.RegisterGetFileNameCb(Test);
  wl.Start();
  pthread_exit(0);
}
