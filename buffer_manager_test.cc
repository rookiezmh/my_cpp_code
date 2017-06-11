#include "buffer_manager.h"
#include <iostream>
#include <set>

using namespace std;
using namespace util;

int main() {

  BufferManager<set<int> *> psbm;
  cout << sizeof(psbm) << endl;

  BufferManager<set<int>> sbm;
  cout << sizeof(sbm) << endl;

  set<int> *ps1 = psbm.GetUse();
  for (int i = 0; i < 10; ++i)
    ps1->insert(i);

  ps1 = psbm.GetUpd();
  for (int i = 0; i < 20; ++i)
    ps1->insert(i);

  cout << psbm.Info() << endl;
  psbm.Swap();
  cout << psbm.Info() << endl;

  set<int> *ps2 = sbm.GetUse();
  for (int i = 0; i < 10; ++i)
    ps2->insert(i);

  ps2 = sbm.GetUpd();
  for (int i = 0; i < 20; ++i)
    ps2->insert(i);
  //cout << sbm.Info() << endl;
  sbm.Swap();
  //cout << sbm.Info() << endl;
}
