#include "jstring.h"

jstring::jstring()
{
}

jstring jstring::trim() const
{
    auto first = find_first_not_of(' ');
    auto last = find_last_not_of(' ');

    return substring(first == npos ? size() : first, last == npos ? last : last+1);
}
