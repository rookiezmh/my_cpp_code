#ifndef GALAXY_HASHER_H_
#define GALAXY_HASHER_H_

#include <stdint.h>

namespace galaxy {

uint32_t Hash(const char *str) {
    uint32_t hash = 5381;
    char c;

    while ((c = *str++) != '\0')
        hash = ((hash << 5) + hash) + static_cast<uint32_t>(c); /* hash * 33 + c */

    return hash;
}

uint32_t Hash(int i) {
    return (uint32_t) i;
}


}

#endif // GALAXY_HASHER_H_
