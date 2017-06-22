#ifndef GALAXY_UTIL_FUNCTIONS_H_
#define GALAXY_UTIL_FUNCTIONS_H_

#include <vector>
#include <string>

namespace util {

int SplitStr(const char sp, const char *cstr, std::vector<std::string> &des);

int SplitStr(const char sp, const std::string &str, std::vector<std::string> &des);

} // namespace util

#endif // GALAXY_UTIL_FUNCTIONS_H_
