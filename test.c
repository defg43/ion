#include "include/ion.h"
#include <stdio.h>
#include <stdbool.h>

bool test() {
    obj_t obj = createEmptyObject();
    insertStringEntry(obj, string("hello"), string("world"));
    printf("%s\n", objectToJson(obj));
    destroyObject(obj);
    return true;
}

int main() {
    test();
    return 0;
}