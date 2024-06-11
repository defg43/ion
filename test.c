#include "include/ion.h"
#include <stdio.h>
#include <stdbool.h>

bool test() {
    obj_t obj = createEmptyObject();
    obj = insertStringEntry(obj, string("hello"), string("world"));
    obj = insertNumberEntry(obj, string("number"), makeNumber(2.275));
    array_t arr = createEmptyArray();
    arr = insertIntoArray(arr, objectValue(string("a")));
    arr = insertIntoArray(arr, objectValue(string("b")));
    arr = insertIntoArray(arr, objectValue(string("c")));
    obj = insertArrayEntry(obj, string("array"), arr);
    obj = insertBoolEntry(obj, string("val1"), true);
    obj = insertBoolEntry(obj, string("val2"), false);
    obj = insertNullEntry(obj, string("val3"));

    string json = objectToJson(obj, (string) {NULL});

    printf("%s\n", json);
    printf("allocated size for json: %ld\n", stringbytesalloced(json));
    destroyObject(obj);
    destroyString(json);
    return true;
}

int main() {
    test();
    return 0;
}
