#pragma once

#include <EASTL/functional.h>

class StringUtils {
public:
    static bool Contains(const char *str, char ch);
    static int FirstOccuranceIndex(const char *str, char ch, int startIndex);

    static void cb_SplitIntoRegions(const char *str,
                                    char seperator,
                                    int maxLen,
                                    const eastl::function<void(int index, int length)> &regionCallback);

    static constexpr bool IsTextDec(char c) {
        return (c >= 'A' && c <= 'z') || (c >= '0' && c <= '9');
    }
};
