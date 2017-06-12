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
  pthread_t x = wl.GetPid();
  pthread_join(x, NULL);
  string s1 = "public:";
  string s2 = "fuck";
  if (wl.InList(s1))
    cout << s1 << " in the list" << endl;
  if (!wl.InList(s2))
    cout << s2 << " not in the list" << endl;
}
