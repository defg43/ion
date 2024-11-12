#ifndef _ION_H_
#define _ION_H_

/*
	ion.h
	ion stands for internal object notation
	it is fully compliant with the json specification
*/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../str/include/str.h"

typedef struct object_t object_t;
typedef struct obj_t_key_t obj_t_key_t;
typedef struct obj_t_value_t obj_t_value_t;
typedef void (*destructor_t)(void *ptr);

typedef struct {
    obj_t_value_t *array;
    size_t allocated_bytes;
    size_t element_size;
    size_t count;
	destructor_t destructor;
} array_t;

typedef enum : int8_t {
	number_t_uint64_t = 0,
	number_t_int64_t = 1,
	number_t_long_double = 2,
	number_t_double = 3,
	number_t_float = 4,
} number_t_discriminant_t;

typedef struct {
	number_t_discriminant_t number_discriminant;
	union {
		uint64_t as_uint64_t;
		int64_t as_int64_t;
		long double as_long_double;
		double as_double;
		float as_float;
	};
} number_t;

typedef enum : int8_t {
	obj_t_null = -1,
	obj_t_false = 0,
	obj_t_true = 1,
	obj_t_number = 2,
	obj_t_string = 3,
	obj_t_array = 4,
	obj_t_obj = 5,	 	
} obj_t_discriminant_t;


struct object_t {
	obj_t_key_t *key;
	obj_t_value_t *value;
	size_t count;
	destructor_t destructor;
};

struct obj_t_key_t {
	// the destructor is implied by the string type, but migth be defined here in future
	string name;
};

struct obj_t_value_t {
	obj_t_discriminant_t discriminant;
	union {
		string str;
		array_t arr;
		number_t num;
		object_t obj;
	};
};

#define makeNumber(num_) 																						\
	({																											\
		auto num = (num_);																						\
		_Generic((num),																							\
			uint8_t:  (number_t) { .number_discriminant = number_t_uint64_t, .as_uint64_t = (uint64_t)num }, 	\
			uint16_t: (number_t) { .number_discriminant = number_t_uint64_t, .as_uint64_t = (uint64_t)num }, 	\
			uint32_t: (number_t) { .number_discriminant = number_t_uint64_t, .as_uint64_t = (uint64_t)num }, 	\
			uint64_t: (number_t) { .number_discriminant = number_t_uint64_t, .as_uint64_t = (uint64_t)num }, 	\
			int8_t:  (number_t) { .number_discriminant = number_t_int64_t, .as_int64_t = (int64_t)num }, 		\
			int16_t: (number_t) { .number_discriminant = number_t_int64_t, .as_int64_t = (int64_t)num }, 		\
			int32_t: (number_t) { .number_discriminant = number_t_int64_t, .as_int64_t = (int64_t)num }, 		\
			int64_t: (number_t) { .number_discriminant = number_t_int64_t, .as_int64_t = (int64_t)num }, 		\
			long double: 																						\
			(number_t) { .number_discriminant = number_t_long_double, .as_long_double = (long double)num }, 	\
			double: (number_t) { .number_discriminant = number_t_double, .as_double = (double)num },			\
			float: (number_t) { .number_discriminant = number_t_float, .as_float = (float)num }					\
		);																										\
	})

#define objectValue(val_) 																					\
	({																										\
		typeof(val_) val = (val_);																			\
		_Generic((val),																						\
			string: (obj_t_value_t) { .discriminant = obj_t_string, .str = coerce(val, string) },			\
			array_t: (obj_t_value_t) { .discriminant = obj_t_array, .arr = coerce(val, array_t) },			\
			number_t: (obj_t_value_t) { .discriminant = obj_t_number, .num = coerce(val, number_t) },		\
			obj_t: (obj_t_value_t) { .discriminant = obj_t_obj, .obj = coerce(val, obj_t) }					\
		);																									\
	})

#define ionInsert(object_, key_, value_) 																	\
({																											\
	obj_t object = (object_);																				\
	typeof(key_) key = (key_); 																				\
	typeof(value_) value = (value_); 																		\
	_Generic((value), 																						\
		string: insertStringEntry(object, string(key), coerce(string(value), string)),						\
		char *: insertStringEntry(object, string(key), coerce(string(value), string)),						\
		const char *: insertStringEntry(object, string(key), coerce(string(value), string)),				\
		array_t: insertArrayEntry(object, string(key), coerce(value, array_t)),								\
		number_t: insertNumberEntry(object, string(key), coerce(makeNumber(value), number_t)),				\
		obj_t: insertSubobjectEntry(object, string(key), coerce(value, obj_t)),								\
		bool: insertBoolEntry(object, string(key), coerce(value, bool)),									\
		nullptr_t: insertNullEntry(object, string(key)),													\
	);																										\
})

object_t createEmptyObject(void);
object_t insertObjectEntry(object_t obj, string key, obj_t_value_t value);
object_t insertSubobjectEntry(object_t obj, string key, object_t value);
object_t insertArrayEntry(object_t obj, string key, array_t value);
object_t insertNumberEntry(object_t obj, string key, number_t value);
object_t insertStringEntry(object_t obj, string key, string value);
object_t insertBoolEntry(object_t obj, string key, bool value);
object_t insertNullEntry(object_t obj, string key);

void destroyObject(object_t obj);

string objectToJson(object_t obj);
string arrayToJson(array_t arr);
string numberToString(number_t number);

object_t jsonToObject(string json_string);

bool parseKey(string json, size_t *pos, string *result);
bool parseValue(string json, size_t *pos, obj_t_value_t *result);
bool parseNumber(string json, size_t *pos, obj_t_value_t *result);
bool parseString(string json, size_t *pos, obj_t_value_t *result);
bool parsePrimitive(string json, size_t *pos, obj_t_value_t *result);
bool parseObject(string json, size_t *pos, obj_t_value_t *result);
bool parseArray(string json, size_t *pos, obj_t_value_t *result);

array_t createEmptyArray(void);
array_t insertIntoArray(array_t arr, obj_t_value_t value);
void destroyArray(array_t arr);

#endif // _ION_H_
