#include "jstring.h"

std::vector<jstring> jstring::split(char c) const
{
    std::vector<jstring> ret;

    for (unsigned i = 0; i < size(); i++) {
        if ((*this)[i] != c) {
            auto start = i;
            for (i = i + 1; i < size() && (*this)[i] != c; i++) {

            }
            ret.push_back(jstring(*this, start, i-start));
        }
    }

    return ret;
}

jstring jstring::trim() const
{
    auto first = find_first_not_of(' ');
    auto last = find_last_not_of(' ');

    return substring(first == npos ? size() : first, last == npos ? last : last+1);
}
