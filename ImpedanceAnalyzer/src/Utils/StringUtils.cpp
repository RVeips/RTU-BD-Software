#include <Utils/StringUtils.hpp>
#include <cstring>

int StringUtils::FirstOccuranceIndex(const char *str, char ch, int startIndex) {
    for (register int i = startIndex; i <= strlen(str); i++) {
        if (str[i] == ch) {
            return i;
        } else if (str[i] == '\0') {
            return -1;
        }
    }

    return -1;
}

bool StringUtils::Contains(const char *str, char ch) {
    for (register int i = 0; i < strlen(str); i++) {
        if (str[i] == ch)
            return true;
    }
    return false;
}

void StringUtils::cb_SplitIntoRegions(const char *str,
                                      char seperator,
                                      int maxLen,
                                      const eastl::function<void(int index, int length)> &regionCallback) {
    bool containsSeperator = Contains(str, seperator);
    if (containsSeperator) {
        int occ     = 0;
        int lastOcc = 0;
        while (1 < 2) {
            lastOcc = occ;
            occ     = FirstOccuranceIndex(str, seperator, occ);

            if ((occ > maxLen) || ((occ == -1) && (lastOcc >= maxLen)))
                return;

            if (occ != -1) {
                regionCallback(lastOcc, occ - lastOcc);
            } else {
                regionCallback(lastOcc, strlen(str) - lastOcc);
                return;
            }

            occ++;
        }
    } else {
        regionCallback(0, strlen(str));
    }
}
