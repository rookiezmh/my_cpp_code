#include "dynamic_words_list.h"
#include <pthread.h>
#include <string>
#include <fstream>
#include <set>
#include <iostream>

using namespace std;

string Test(void *arg) {
  return "./dynamic_words_list.h";
}

void Update(ifstream &in, set<string> &s) {
  string line;
  while (getline(in, line)) {
    s.insert(line);
  }
}

int main() {
  util::DynamicWordsList<string> sl(5); 
  sl.RegisterGetFileNameCb(Test);
  sl.RegisterUpdateCb(Update);
  sl.Start();
  cout << "read file done. Test it\n";
  string s1("  bool InList(const T &w) const;");
  cout << s1 << (sl.InList(s1) ? " in" : " not in") << " list\n";
  pthread_join(sl.GetPid(), NULL);
}
