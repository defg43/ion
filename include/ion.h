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

typedef struct obj_t obj_t;
typedef struct obj_t_key_t obj_t_key_t;
typedef struct obj_t_value_t obj_t_value_t;

typedef struct {
    obj_t *array;
    size_t allocated_bytes;
    size_t element_size;
    size_t count;
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

typedef void (*destructor_t)(void *ptr);

struct obj_t {
	obj_t_key_t *key;
	obj_t_value_t *value;
	size_t count;
	destructor_t destructor;
};

struct obj_t_key_t {
	destructor_t destructor;
	string name;
};

struct obj_t_value_t {
	destructor_t destructor;
	obj_t_discriminant_t discriminant;
	union {
		string str;
		array_t arr;
		number_t num;
		obj_t obj;
	};
};


obj_t createEmptyObject(void);
obj_t insertObjectEntry(obj_t obj, string key, obj_t_value_t value);
obj_t insertSubobjectEntry(obj_t obj, string key, obj_t value);
obj_t insertArrayEntry(obj_t obj, string key, array_t value);
obj_t insertNumberEntry(obj_t obj, string key, number_t value);
obj_t insertStringEntry(obj_t obj, string key, string value);
obj_t insertBoolEntry(obj_t obj, string key, bool value);
obj_t insertNullEntry(obj_t obj, string key);
#endif // _ION_H_
