#include "../include/ion.h"
#include "../witc/foreach.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

// this prototypes are only function internal could be exported in future
void consumeWhitespace(string *json, size_t *pos);

object_t createEmptyObject() {
    return (object_t) {
        .key = NULL,
        .value = NULL,
        .count = 0,
        .destructor = free,
    };
};

object_t insertObjectEntry(object_t object, string key, obj_t_value_t value) {
    object.key = realloc(object.key, sizeof(obj_t_key_t) * (object.count + 1));
    object.value = realloc(object.value, sizeof(obj_t_value_t) * (object.count + 1));
    if(object.key == NULL || object.value == NULL) {
        fprintf(stderr, "failed to allocate memory in insertObjectEntry\n");
        exit(EXIT_FAILURE);
    } 
    object.key[object.count] = key; // key is just absorbed here and owned here from now on
    // this should be sourced from the string allocator backend once it is implemented
    object.value[object.count] = value;
    object.count++;
    return object;
}

object_t insertSubobjectEntry(object_t object, string key, object_t value) {
    printf("there are currently %ld entries in the object\n", object.count);
    string *key_ = realloc(object.key, sizeof(string) * (object.count + 1));
    obj_t_value_t *value_ = realloc(object.value, sizeof(obj_t_value_t) * (object.count + 1));
    if(key_ == NULL || value_ == NULL) {
        fprintf(stderr, "failed to allocate memory in insertSubobjectEntry\n");
        exit(EXIT_FAILURE);
    } 
    object.key = key_;
    object.value = value_;
    object.key[object.count] = key; // key is just absorbed here and owned here from now on
    // this should be sourced from the string allocator backend once it is implemented
    object.value[object.count].discriminant = obj_t_obj;
    object.value[object.count].obj = value;
    object.count++;
    return object;
}

object_t insertArrayEntry(object_t object, string key, array_t value) {
    string *key_ = realloc(object.key, sizeof(string) * (object.count + 1));
    obj_t_value_t *value_ = realloc(object.value, sizeof(obj_t_value_t) * (object.count + 1));
    if(key_ == NULL || value_ == NULL) {
        fprintf(stderr, "failed to allocate memory in insertArrayEntry\n");
        exit(EXIT_FAILURE);
    } 
    object.key = key_;
    object.value = value_;
    object.key[object.count] = key; // key is just absorbed here and owned here from now on
    // this should be sourced from the string allocator backend once it is implemented
    object.value[object.count].discriminant = obj_t_array;
    object.value[object.count].arr = value;
    object.count++;
    return object;
}

object_t insertNumberEntry(object_t object, string key, number_t value) {
    string *key_ = realloc(object.key, sizeof(string) * (object.count + 1));
    obj_t_value_t *value_ = realloc(object.value, sizeof(obj_t_value_t) * (object.count + 1));
    if(key_ == NULL || value_ == NULL) {
        fprintf(stderr, "failed to allocate memory in insertNumberEntry\n");
        exit(EXIT_FAILURE);
    } 
    object.key = key_;
    object.value = value_;
    object.key[object.count] = key; // key is just absorbed here and owned here from now on
    // this should be sourced from the string allocator backend once it is implemented
    object.value[object.count].discriminant = obj_t_number;
    object.value[object.count].num = value;
    object.count++;
    return object;
}

object_t insertStringEntry(object_t object, string key, string value) {
    string *key_ = realloc(object.key, sizeof(obj_t_key_t) * (object.count + 1));
    obj_t_value_t *value_ = realloc(object.value, sizeof(obj_t_value_t) * (object.count + 1));
    printf("key: %p\nvalue: %p\n", key, value);
    if(key_ == NULL || value_ == NULL) {
        fprintf(stderr, "failed to allocate memory in insertStringEntry\n");
        exit(EXIT_FAILURE);
    } 
    object.key = key_;
    object.value = value_;
    printf("the object entry count is %ld\n", object.count);
    printf("%p\n", object.key);
    object.key[object.count] = key; // key is just absorbed here and owned here from now on
    // this should be sourced from the string allocator backend once it is implemented
    object.value[object.count].discriminant = obj_t_string;
    object.value[object.count].str = value;
    object.count++;
    return object;
}

object_t insertBoolEntry(object_t object, string key, bool value) {
    string *key_ = realloc(object.key, sizeof(string) * (object.count + 1));
    obj_t_value_t *value_ = realloc(object.value, sizeof(obj_t_value_t) * (object.count + 1));
    if(key_ == NULL || value_ == NULL) {
        fprintf(stderr, "failed to allocate memory in insertBoolEntry\n");
        exit(EXIT_FAILURE);
    } 
    object.key = key_;
    object.value = value_;
    object.key[object.count] = key; // key is just absorbed here and owned here from now on
    // this should be sourced from the string allocator backend once it is implemented
    if(value) {
        object.value[object.count].discriminant = obj_t_true;
    } else {
        object.value[object.count].discriminant = obj_t_false;
    }
    object.count++;
    return object;
}

object_t insertNullEntry(object_t object, string key) {
    string *key_ = realloc(object.key, sizeof(string) * (object.count + 1));
    obj_t_value_t *value_ = realloc(object.value, sizeof(obj_t_value_t) * (object.count + 1));
    if(key_ == NULL || value_ == NULL) {
        fprintf(stderr, "failed to allocate memory in insertNullEntry\n");
        exit(EXIT_FAILURE);
    } 
    object.key = key_;
    object.value = value_;
    object.key[object.count] = key; // key is just absorbed here and owned here from now on
    // this should be sourced from the string allocator backend once it is implemented
    object.value[object.count].discriminant = obj_t_null;
    object.count++;
    return object;
}

string arrayToJson(array_t array) {
    string ret;
        ret =  string("[");
    for (size_t i = 0; i < array.count; i++) {
        switch(array.array[i].discriminant) {
            case obj_t_string:
                ret = append(ret, "\"");
                ret = append(ret, array.array[i].str);
                ret = append(ret, "\"");
            break;
            case obj_t_array:
                ret = arrayToJson(array.array[i].arr);
            break;
            case obj_t_obj: 
                ret = objectToJson(array.array[i].obj);
            break;
            case obj_t_null:
            	ret = append(ret, "null");
            break;
            case obj_t_true:
            	ret = append(ret, "true");
            break;
            case obj_t_false:
            	ret = append(ret, "false");
            break;
            case obj_t_number:
                string temp = numberToString(array.array[i].num);
                ret = append(ret, temp);
                destroyString(temp);
            break;
            default:
        }
        if(i < array.count - 1) {
            ret = append(ret, ", ");
        }
    }
    ret = append(ret, "]");
    return ret;
}

void consumeWhitespace(string *json, size_t *pos) {
    while (*pos < stringlen(*json) && 
          (json->at[*pos] == ' ' || json->at[*pos] == '\n' || json->at[*pos] == '\t')) {
        (*pos)++;
    }
}


bool parseKey(string json, size_t *pos, string *result) {
	bool success = false;
	obj_t_value_t val = {};
	success = parseString(json, pos, &val);
	if(success) {
		// if there is anything in result destroy it
		*result = val.str;
	}
	return success;
}

 
bool parseValue(string json, size_t *pos, obj_t_value_t *result) {
	size_t starting_pos = *pos;

	ion_json_parser_t parsers[] = {
		parseNumber,
		parseString,
		parsePrimitive,
		parseObject,
		parseArray,
	};	

	foreach(ion_json_parser_t parser of parsers) if(!parser(json, pos, result)) {
		*pos = starting_pos;
	} else {
		return true;
	}
	return false;
}

bool parseNumber(string json, size_t *pos, obj_t_value_t *result) {
	bool sign = false;
	bool success = false;
	// x.y
	uint64_t x = 0;
	double y = 0;

	consumeWhitespace(&json, pos);

	if(json.at[*pos] == '-') {
		sign = true;
		(*pos)++;
	}

	if(!isdigit(json.at[*pos])) {
		return false;
	}
		
	while(json.at[*pos] && isdigit(json.at[*pos])) {
		x *= 10;
		x += json.at[*pos] - '0';
		(*pos)++;
	}
	
	if(json.at[*pos] == '.') {
		(*pos)++;
		if(!isdigit(json.at[*pos])) {
			return false;
		}
		while(json.at[*pos] && isdigit(json.at[*pos])) {
			y += json.at[*pos] - '0';
    		y *= 10;
    		(*pos)++;
		}
		while((y /= 10) > 1.0);		
  		y = x + y;
		y = sign ? -y : y;

		*result = (obj_t_value_t) {
			.discriminant = obj_t_number,
			.num = (number_t) {
				.number_discriminant = number_t_double,
				.as_double = y,
			},
		};
		success = true;
		return success;
  		
	} else {

		if(sign) {
			x = -x;
		}

		*result = (obj_t_value_t) {
			.discriminant = obj_t_number,
			.num = sign ? (number_t) {
					.number_discriminant = number_t_uint64_t,
					.as_uint64_t = x,
				} : (number_t) {
					.number_discriminant = number_t_int64_t,
					.as_int64_t = x,
				}
		};
		success = true;
		return true;
	}
	return success;
}

bool parseString(string json, size_t *pos, obj_t_value_t *result) {
	if(json.at[*pos] != '"') {
		return false;
	}
	size_t starting_pos = *pos;
	(*pos)++;
	size_t delta = 0;
	size_t start = *pos;
	while((*pos)++, delta++, json.at[*pos] && (json.at[*pos] != '"' || json.at[*pos - 1] == '\\'));
	if(json.at[*pos] == '"') {
		string substr = sliceFromString(json, start, start + delta);
		*result = (obj_t_value_t) {
			.discriminant = obj_t_string,
			.str = substr,
		};
		(*pos)++; // skipping the last "
		return true;
	} else {
		*pos = starting_pos;
		return false;
	}
}

bool parsePrimitive(string json, size_t *pos, obj_t_value_t *result) {
	if(streql(&json.at[*pos], "true")) {
		*result = (obj_t_value_t) {
			.discriminant = obj_t_true,
		};
		*pos += 4;
		return true;
	}
	
	if(streql(&json.at[*pos], "false")) {
		*result = (obj_t_value_t) {
			.discriminant = obj_t_false,
		};
		*pos += 5;
		return true;
	}
	
	if(streql(&json.at[*pos], "null")) {
		*result = (obj_t_value_t) {
			.discriminant = obj_t_null,
		};
		*pos += 4;
		return true;
	}
	return false;
}

bool parseObject(string json, size_t *pos, obj_t_value_t *result) {
	size_t starting_pos = *pos;
	consumeWhitespace(&json, pos);
	if(json.at[*pos] == '{') {
		(*pos)++;
	} else {
		return false;
	}
	
	// iterate over entires

	while(true) {
		consumeWhitespace(&json, pos);
		string key = {};
		obj_t_value_t val = {};

		if(json.at[*pos] == '}') {
			goto success;
		}
	
		if(parseKey(json, pos, &key)) {} else {
			goto syntax_error;
		}

		consumeWhitespace(&json, pos);

		if(json.at[*pos] == ':') {
			(*pos)++;
		} else {
			goto syntax_error;
		}

		consumeWhitespace(&json, pos);

		if(parseValue(json, pos, &val)) {
			result->obj = insertObjectEntry(result->obj, key, val);
		} else {
			goto syntax_error;
		}

		consumeWhitespace(&json, pos);

		if(json.at[*pos] == ',') {
			(*pos)++;
			continue;
		} else if(json.at[*pos] == '}') {
			goto success;
		} else {
			goto syntax_error;
		}
	}
	syntax_error:
		destroyObject(result->obj);
		*pos = starting_pos;
		return false;
	success:
		(*pos)++;
		result->discriminant = obj_t_obj;
		return true;
}

bool parseArray(string json, size_t *pos, obj_t_value_t *result) {
	size_t starting_pos = *pos;
	if(json.at[*pos] == '[') {
		(*pos)++;
	} else {
		return false;
	}
	array_t arr = createEmptyArray();

	while(true) {
		obj_t_value_t val = {};
		consumeWhitespace(&json, pos);		
		if(json.at[*pos] == ']') {
			goto success;
		}
		
		if(parseValue(json, pos, &val)) {
			arr = insertIntoArray(arr, val);
		} else {
			goto syntax_error;
		}

		consumeWhitespace(&json, pos);

		if(json.at[*pos] == ',') {
			(*pos)++;
			continue;
		} else if(json.at[*pos] == ']') {
			goto success;
		} else {
			goto syntax_error;
		}
	}
	syntax_error:
		destroyArray(arr);
		*pos = starting_pos;
		return false;
	success:
		(*pos)++;
		result->arr = arr;
		result->discriminant = obj_t_array;
		return true;
}

object_t jsonToObject(string json_string) {
	object_t ret = createEmptyObject();
	obj_t_value_t val = {};
	size_t pos = 0;
	if(parseObject(json_string, &pos, &val)) {
		ret = val.obj;
	}
	return ret;		
}

string objectToJson(object_t object) {
    string ret;
    ret =  string("{");
    printf("json: there are currently %ld entries\n", object.count);
    for (size_t i = 0; i < object.count; i++) {
        ret = append(ret, "\"");
        ret = append(ret, object.key[i]);
        ret = append(ret, "\"");
        ret = append(ret, " : ");

        switch(object.value[i].discriminant) {
            case obj_t_string:
                ret = append(ret, "\"");
                ret = append(ret, object.value[i].str);
                ret = append(ret, "\"");
            break;
            case obj_t_array:
                string array_result = arrayToJson(object.value[i].arr);
                ret = append(ret, array_result);
                destroyString(array_result);
            break;
            case obj_t_obj:
                string object_result = objectToJson(object.value[i].obj);
                ret = append(ret, object_result);
                destroyString(object_result);
            break;
            case obj_t_null:
                ret = append(ret, "null");
            break;
            case obj_t_true:
                ret = append(ret, "true");
            break;
            case obj_t_false:
                ret = append(ret, "false");
            break;
            case obj_t_number:
                string temp = numberToString(object.value[i].num);
                ret = append(ret, temp);
                destroyString(temp);
            break;
            default:
                ret = append(ret, "null");
            break;
        }
        if(i != object.count - 1) {
            ret = append(ret, ", ");        
        }
    }

    ret = append(ret, "}");
    return ret;
}

string numberToString(number_t number) {
    string ret = string("");
    char digit[2] = { '0', '\0' };
    bool needs_sign = false;
    switch(number.number_discriminant) {
        case number_t_uint64_t:
            while(number.as_uint64_t > 0) {
                digit[0] = '0' + number.as_uint64_t % 10;
                number.as_uint64_t /= 10;
                ret = append(ret, digit);
            }
            stringReverse(ret);
        break;
        case number_t_int64_t:
            if(number.as_int64_t < 0) {
                number.as_int64_t = -number.as_int64_t;
                needs_sign = true;
            }
            while(number.as_int64_t > 0) {
                digit[0] = '0' + number.as_int64_t % 10;
                number.as_int64_t /= 10;
                ret = append(ret, digit);
            }
            if(needs_sign) {
                ret = append(ret, "-");
                needs_sign = false;
            }
            stringReverse(ret);
        break;
        case number_t_float:
            if(number.as_float < 0) {
                number.as_float = -number.as_float;
                needs_sign = true;
            }
            {
                // extra 3 for null terminator, . and sign
                #define MAX_FLOAT_DIGITS (32 * 2 + 3)
                // man, i sure do hope this fixed stack buffer is enough to hold my string
                // what could possibly go wrong?
                char num_buff[MAX_FLOAT_DIGITS]; 
                snprintf(num_buff, MAX_FLOAT_DIGITS, "%.6g", number.as_float);
                #undef MAX_FLOAT_DIGITS
                if(needs_sign) {
                    ret = append(ret, "-");
                    needs_sign = false;
                }
                ret = append(ret, num_buff);
            }
        break;
        case number_t_double:
            if(number.as_double < 0) {
                number.as_double = -number.as_double;
                needs_sign = true;
            }
            {
                // extra 3 for null terminator, . and sign
                #define MAX_FLOAT_DIGITS (32 * 4 + 3)
                // man, i sure do hope this fixed stack buffer is enough to hold my string
                // what could possibly go wrong?
                char num_buff[MAX_FLOAT_DIGITS]; 
                snprintf(num_buff, MAX_FLOAT_DIGITS, "%.15g", number.as_double);
                #undef MAX_FLOAT_DIGITS
                if(needs_sign) {
                    ret = append(ret, "-");
                    needs_sign = false;
                }
                ret = append(ret, num_buff);
            }
        break;
        case number_t_long_double:
            if(number.as_long_double < 0) {
                number.as_long_double = -number.as_long_double;
                needs_sign = true;
            }
            {
                // extra 3 for null terminator, . and sign
                #define MAX_FLOAT_DIGITS (32 * 6 + 3)
                // man, i sure do hope this fixed stack buffer is enough to hold my string
                // what could possibly go wrong?
                char num_buff[MAX_FLOAT_DIGITS]; 
                snprintf(num_buff, MAX_FLOAT_DIGITS, "%.18g", number.as_long_double);
                #undef MAX_FLOAT_DIGITS
                if(needs_sign) {
                    ret = append(ret, "-");
                    needs_sign = false;
                }
                ret = append(ret, num_buff);
            }
        
        default:
            fprintf(stderr, "unknown number discriminant\n");
            exit(EXIT_FAILURE);
        break;
    }
    return ret;
}

void destroyArray(array_t array) {
    for(size_t i = 0; i < array.count; i++) {
        switch(array.array[i].discriminant) {
            case obj_t_string:
                destroyString(array.array[i].str);
                break;
            case obj_t_array:
                destroyArray(array.array[i].arr);
                break;
            case obj_t_obj:
                destroyObject(array.array[i].obj);
                break;
            default:
                break;
        }
    }
    if(array.destructor == NULL) {
        array.destructor = free;
        fprintf(stderr, 
            "setting array destructor to free in destructor because it was set to NULL previously\n");
    }
    array.destructor(array.array);
}

void destroyObject(object_t object) { 
    for(size_t i = 0; i < object.count; i++) {
        destroyString(object.key[i]);
        switch(object.value[i].discriminant) {
            case obj_t_string:
                destroyString(object.value[i].str);
                break;
            case obj_t_array:
                destroyArray(object.value[i].arr);
                break;
            case obj_t_obj:
                destroyObject(object.value[i].obj);
                break;
            default:
                break;
        }
    }
    if(object.destructor == NULL) {
        object.destructor = free;
        fprintf(stderr, 
            "setting object destructor to free in destructor because it was set to NULL previously\n");
    }
    object.destructor(object.key);
    object.destructor(object.value);
}

array_t createEmptyArray(void) {
    return (array_t) {
        .count = 0,
        .allocated_bytes = 0,
        .destructor = NULL,
        .element_size = sizeof(obj_t_value_t),
    };
}

array_t insertIntoArray(array_t arr, obj_t_value_t value) {
    obj_t_value_t *value_ = realloc(arr.array, sizeof(obj_t_value_t) * (arr.count + 1));
    if(value_ == NULL) {
        fprintf(stderr, "failed to allocate memory in insertIntoArray\n");
        exit(EXIT_FAILURE);
    } 
    if(arr.destructor == NULL) {
        arr.destructor = free; 
    } else if(arr.destructor != free){
        fprintf(stderr, "using different memory managment schemes on the same array\n");
        exit(EXIT_FAILURE);
    }
    arr.count++;
    arr.array = value_;
    arr.array[arr.count - 1] = value;
    return arr;
}

int objcmp(object_t obj1, object_t obj2) {
    size_t count = obj1.count < obj2.count ? obj1.count : obj2.count;
    return objncmp(obj1, obj2, count);
}

int objncmp(object_t obj1, object_t obj2, size_t n) {
    size_t i = 0;
    while(i < obj1.count && i < obj2.count && i < n) {
        // compare both keys and values
        int val_res = valcmp(obj1.value[i], obj2.value[i]);
        int str_res = stringcmp(obj1.key[i], obj2.key[i]);

        if(val_res == 0 && str_res == 0) {
            i++;
            continue;
        } else {
            if(val_res != 0) {
                return val_res;
            } else {
                return str_res;
            }
        }
    }
    return 0;
}

bool objeql(object_t obj1, object_t obj2) {
    return objncmp(obj1, obj2, obj1.count) == 0;
}

bool objneql(object_t obj1, object_t obj2, size_t n) {
    return objncmp(obj1, obj2, n) == 0;
}

bool objcontains(object_t obj, string key) {
    size_t n = obj.count;
    while(n --> 0) {
        if(stringeql(obj.key[n], key)) {
            return true;
        }
    }
    return false;
}

obj_t_value_t objget(object_t obj, string key) {
    size_t n = obj.count;
    while(n --> 0) {
        if(stringeql(obj.key[n], key)) {
            return obj.value[n];
        }
    }
    return (obj_t_value_t) {
        .discriminant = obj_t_null,
    }; 
}

object_t objcopy(object_t obj);
bool objremove(object_t obj, string key);

int arraycmp(array_t arr1, array_t arr2);
int arrayncmp(array_t arr1, array_t arr2, size_t n);
bool arrayeql(array_t arr1, array_t arr2);
bool arrayneql(array_t arr1, array_t arr2, size_t n);
obj_t_value_t arrayget(array_t arr, size_t index);
array_t arraycopy(array_t arr);

int valcmp(obj_t_value_t val1, obj_t_value_t val2);
int valncmp(obj_t_value_t val1, obj_t_value_t val2, size_t n);
bool valeql(obj_t_value_t val1, obj_t_value_t val2);
bool valneql(obj_t_value_t val1, obj_t_value_t val2, size_t n);

