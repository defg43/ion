#include "include/ion.h"
#include <stdio.h>
#include <stdbool.h>

bool test() {
    string str = stringFromString(stringFromCharPtr("test"));
    printf("%s\n", str);
    destroyString(str);
    return true;
}

int main() {
    test();
    return 0;
}