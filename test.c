#include "include/ion.h"
#include <stdio.h>
#include <stdbool.h>

bool test() {
    obj_t obj = createEmptyObject();
    insertStringEntry(obj, string("hello"), string("world"));
}

int main() {
    test();
    return 0;
}