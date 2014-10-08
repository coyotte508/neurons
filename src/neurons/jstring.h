#ifndef JSTRING_H
#define JSTRING_H

#include <string>
#include <cstring>

/* Simple wrapper of std::string, for more user-friendly functions.
 * Name inspired of "js string".
 */
class jstring : public std::string
{
public:
    jstring();
    jstring(const std::string &s): std::string(s) {}

    bool starts_with(char c) const { return length() > 0 && at(0) == c; }
    bool starts_with(const char *str) const { return compare(0, strlen(str), str) == 0; }
    bool starts_with(const std::string & str) const { return compare(0, str.length(), str) == 0; }

    jstring substring(size_type start, size_type end = std::string::npos) const { return substr(start, end-start); }

    jstring trim() const;
};

#endif // JSTRING_H
