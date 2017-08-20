#pragma once

#include <algorithm>

const char nums[] = "0123456789";

template<typename T>
size_t Convert(char buf[], T value) {
    T i = value;
    char *p = buf;
    do {
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = nums[lsd];
    } while (i != 0);
    if (value < 0) {
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);
    return p - buf;
}
