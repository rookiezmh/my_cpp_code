#include "util_functions.h"
#include <string.h>

namespace util {

using namespace std;

int SplitStr(const char sp, const char *cstr, vector<string> &dest) {
  size_t len;
  if (cstr == NULL || (len = strlen(cstr)) == 0) {
    return 0;
  }
  dest.clear();
  const char *b = cstr;
  const char *p = strchr(cstr, sp);
  while (p != NULL) {
    dest.push_back(string(cstr, p));
    cstr = p + 1;
    p = strchr(cstr, sp);
  }
  dest.push_back(string(cstr, b + len));
  return dest.size();
}

int SplitStr(const char sp, const string &str, vector<string> &dest) {
  return SplitStr(sp, str.c_str(), dest);
}

} // namespace util
