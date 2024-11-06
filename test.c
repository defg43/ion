
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

#define literal(x...) literal_defered(x)
#define literal_defered(x...) #x

bool test2() {
	string my_json_string = string(literal({"a" : "b", "c" : "d"}));
	obj_t testobj = jsonToObject(my_json_string);
	printf("the original json string is %s\n", my_json_string);
	string new_json_string = string("");
	new_json_string = objectToJson(testobj, new_json_string);
	printf("the new string is %s\n", new_json_string);
	printf("inside of the object there is the key >%s<\n", testobj.key[0]);

//	destroyString(new_json_string);
//	destroyString(my_json_string);
//	destroyObject(testobj);
	return true;	
}

bool test3() {
	string num = string("1234.5678");
	size_t pos = 0;
	obj_t_value_t result;
	printf("parsing successful: %s\n", parseNumber(num, &pos, &result) ? "true" : "false");
	printf("the result was %0.17f | %ld\n", result.num.as_double, result.num.as_int64_t);

	destroyString(num);

	return true;
}

bool test4() {
	string str = string("\"test here\\\"\"");
	size_t pos = 0;
	obj_t_value_t result;

	printf("parsing successful: %s\n", parseString(str, &pos, &result) ? "true" : "false");
	printf("the result was %s\n", result.str);
	
	destroyString(str);
}

int main() {
    test4();
    return 0;
}
