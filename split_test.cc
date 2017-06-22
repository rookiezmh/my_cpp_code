#include "util_functions.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
  fstream f;
  f.open(argv[1]);
  string line;
  while (getline(f, line)) {
    vector<string> v;
    util::SplitStr(' ', line, v);
    cout << line << endl;
    for (auto s : v) {
      cout << s << "\t";
    }
    cout << "\n";
  }
}
