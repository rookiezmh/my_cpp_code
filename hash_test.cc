#include "hash_manager.h"
#include <stdio.h>

using galaxy::HashManager;
struct Val {
    int x;
    char y[32];
};

int main() {
    HashManager<int, Val> map;
    Val v[] = {{1, "This is 1"}, {1, "This is 2"}};
    map.Insert(1, v[0]);
    map.Insert(2, v[1]);
    map.FinishHash();
    Val tmp;
    if (!map.FindItem(1, tmp)) {
        printf("%s\n", tmp.y);
    }
    if (!map.FindItem(2, tmp)) {
        printf("%s\n", tmp.y);
    }
}
