#include "dynamic_words_list.h"
#include <iostream>
#include <string>
#include <unistd.h>

namespace util {

using namespace std;

void *DynamicWordsList::UpdateRoutine(void *arg) {
  DynamicWordsList *p = (DynamicWordsList *) arg;
  const string file_name = p->file_name_cb_(p->file_name_cb_arg_); 
  fstream f(file_name);
  char buf[1024];
  set<string> *s = p->sbm_->GetUse();
  while (f) {
    f.getline(buf, sizeof(buf));
    cout << "insert " << buf << endl;
    s->insert(buf);
  }
  return NULL;
}

void DynamicWordsList::Init() {
  file_name_cb_ = NULL;
  file_name_cb_arg_ = NULL;
  update_cb_ = NULL;
}

int DynamicWordsList::Start() {
  return pthread_create(&pid_, NULL, &DynamicWordsList::UpdateRoutine, this);
}

} // namespace util
