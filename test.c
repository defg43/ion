#include "include/ion.h"
#include <stdio.h>
#include <stdbool.h>

bool test() {
    obj_t obj = createEmptyObject();
    obj = insertStringEntry(obj, string("hello"), string("world"));
    obj = insertStringEntry(obj, string("foo"), string("bar"));
    obj = insertStringEntry(obj, string("baz"), string("qux"));
    obj = insertStringEntry(obj, string("cat"), string("dog"));
    obj_t obj2 = createEmptyObject();
    obj2 = insertStringEntry(obj2, string("hello2"), string("world2"));
    obj = insertSubobjectEntry(obj, string("obj2"), obj2);
    obj = insertStringEntry(obj, string("hello3"), string("world3"));
    array_t arr = createEmptyArray();
    arr = insertIntoArray(arr, objectValue(string("a")));
    arr = insertIntoArray(arr, objectValue(string("b")));
    arr = insertIntoArray(arr, objectValue(string("c")));
    obj = insertArrayEntry(obj, string("arr"), arr);

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
