
#include "include/ion.h"
#include "witc/foreach.h"
#include <stdio.h>
#include <stdbool.h>

bool test1() {
    object_t obj = createEmptyObject();
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

    string json = objectToJson(obj);

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
	object_t testobj = jsonToObject(my_json_string);
	printf("the original json string is %s\n", my_json_string);
	string new_json_string = string("");
	new_json_string = objectToJson(testobj);
	printf("the new string is %s\n", new_json_string);
	printf("inside of the object there is the key >%s<\n", testobj.key[0]);

	destroyString(new_json_string);
	destroyString(my_json_string);
	destroyObject(testobj);
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
	return true;
}

bool test5() {
	string n = string("null");
	string t = string("true");
	string f = string("false");

	size_t pos = 0;
	obj_t_value_t result;

	parsePrimitive(n, &pos, &result);
	pos = 0;
	printf("the result discriminant is %d\n", result.discriminant);

	parsePrimitive(t, &pos, &result);
	pos = 0;
	printf("the result discriminant is %d\n", result.discriminant);

	parsePrimitive(f, &pos, &result);
	pos = 0;
	printf("the result discriminant is %d\n", result.discriminant);

	destroyString(n);
	destroyString(t);
	destroyString(f);
	
	return true;
}

bool test6() {
	string json = string(literal({"a" : "b", "c" : [1, 2, 3, "xyz", true, false, null]}));
	object_t obj = createEmptyObject();
	obj = jsonToObject(json);
	string output = objectToJson(obj);
	printf("%s\n", output);

	destroyString(output);
	destroyObject(obj);
	return true;
}

int main() {
	typeof(bool (*)(void)) tests[] = {
		test1, 
		test2, 
		test3, 
		test4, 
		test5, 
		test6,
	};

    foreach(typeof(bool (*)(void)) test of tests) {
    	assert(test());
    }
    return 0;
}
