#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <sstream>
using namespace std;

inline bool is_integer(const string& str) {
    if (str.empty() || ((!isdigit(str[0])) && (str[0] != '-') && (str[0] != '+'))) {
        return false;
    }
    char *p;
    strtol(str.c_str(), &p, 10);
    return (*p == 0);
}
   
#endif // UTIL_H
