#include "../include/ion.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

// this prototypes are only function internal could be exported in future
void consumeWhitespace(string *json, size_t *pos);
string parseString(string json, size_t *pos);

obj_t createEmptyObject() {
    return (obj_t) {
        .key = NULL,
        .value = NULL,
        .count = 0,
        .destructor = free,
    };
};

obj_t insertObjectEntry(obj_t object, string key, obj_t_value_t value) {
    object.key = realloc(object.key, sizeof(obj_t_key_t) * (object.count + 1));
    object.value = realloc(object.value, sizeof(obj_t_value_t) * (object.count + 1));
    if(object.key == NULL || object.value == NULL) {
        fprintf(stderr, "failed to allocate memory in insertObjectEntry\n");
        exit(EXIT_FAILURE);
    } 
    object.key[object.count].name = key; // key is just absorbed here and owned here from now on
    // this should be sourced from the string allocator backend once it is implemented
    object.value[object.count] = value;
    object.count++;
    return object;
}

obj_t insertSubobjectEntry(obj_t object, string key, obj_t value) {
    printf("there are currently %ld entries in the object\n", object.count);
    obj_t_key_t *key_ = realloc(object.key, sizeof(obj_t_key_t) * (object.count + 1));
    obj_t_value_t *value_ = realloc(object.value, sizeof(obj_t_value_t) * (object.count + 1));
    if(key_ == NULL || value_ == NULL) {
        fprintf(stderr, "failed to allocate memory in insertSubobjectEntry\n");
        exit(EXIT_FAILURE);
    } 
    object.key = key_;
    object.value = value_;
    object.key[object.count].name = key; // key is just absorbed here and owned here from now on
    // this should be sourced from the string allocator backend once it is implemented
    object.value[object.count].discriminant = obj_t_obj;
    object.value[object.count].obj = value;
    object.count++;
    return object;
}

obj_t insertArrayEntry(obj_t object, string key, array_t value) {
    obj_t_key_t *key_ = realloc(object.key, sizeof(obj_t_key_t) * (object.count + 1));
    obj_t_value_t *value_ = realloc(object.value, sizeof(obj_t_value_t) * (object.count + 1));
    if(key_ == NULL || value_ == NULL) {
        fprintf(stderr, "failed to allocate memory in insertArrayEntry\n");
        exit(EXIT_FAILURE);
    } 
    object.key = key_;
    object.value = value_;
    object.key[object.count].name = key; // key is just absorbed here and owned here from now on
    // this should be sourced from the string allocator backend once it is implemented
    object.value[object.count].discriminant = obj_t_array;
    object.value[object.count].arr = value;
    object.count++;
    return object;
}

obj_t insertNumberEntry(obj_t object, string key, number_t value) {
    obj_t_key_t *key_ = realloc(object.key, sizeof(obj_t_key_t) * (object.count + 1));
    obj_t_value_t *value_ = realloc(object.value, sizeof(obj_t_value_t) * (object.count + 1));
    if(key_ == NULL || value_ == NULL) {
        fprintf(stderr, "failed to allocate memory in insertNumberEntry\n");
        exit(EXIT_FAILURE);
    } 
    object.key = key_;
    object.value = value_;
    object.key[object.count].name = key; // key is just absorbed here and owned here from now on
    // this should be sourced from the string allocator backend once it is implemented
    object.value[object.count].discriminant = obj_t_number;
    object.value[object.count].num = value;
    object.count++;
    return object;
}

obj_t insertStringEntry(obj_t object, string key, string value) {
    obj_t_key_t *key_ = realloc(object.key, sizeof(obj_t_key_t) * (object.count + 1));
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
    object.key[object.count].name = key; // key is just absorbed here and owned here from now on
    // this should be sourced from the string allocator backend once it is implemented
    object.value[object.count].discriminant = obj_t_string;
    object.value[object.count].str = value;
    object.count++;
    return object;
}

obj_t insertBoolEntry(obj_t object, string key, bool value) {
    obj_t_key_t *key_ = realloc(object.key, sizeof(obj_t_key_t) * (object.count + 1));
    obj_t_value_t *value_ = realloc(object.value, sizeof(obj_t_value_t) * (object.count + 1));
    if(key_ == NULL || value_ == NULL) {
        fprintf(stderr, "failed to allocate memory in insertBoolEntry\n");
        exit(EXIT_FAILURE);
    } 
    object.key = key_;
    object.value = value_;
    object.key[object.count].name = key; // key is just absorbed here and owned here from now on
    // this should be sourced from the string allocator backend once it is implemented
    if(value) {
        object.value[object.count].discriminant = obj_t_true;
    } else {
        object.value[object.count].discriminant = obj_t_false;
    }
    object.count++;
    return object;
}

obj_t insertNullEntry(obj_t object, string key) {
    obj_t_key_t *key_ = realloc(object.key, sizeof(obj_t_key_t) * (object.count + 1));
    obj_t_value_t *value_ = realloc(object.value, sizeof(obj_t_value_t) * (object.count + 1));
    if(key_ == NULL || value_ == NULL) {
        fprintf(stderr, "failed to allocate memory in insertNullEntry\n");
        exit(EXIT_FAILURE);
    } 
    object.key = key_;
    object.value = value_;
    object.key[object.count].name = key; // key is just absorbed here and owned here from now on
    // this should be sourced from the string allocator backend once it is implemented
    object.value[object.count].discriminant = obj_t_null;
    object.count++;
    return object;
}

string arrayToJson(array_t array, string external) {
    string ret;
    if(external.data == NULL) {
        ret =  string("[");
    } else {
        ret = append(external, "[");
    }
    for (size_t i = 0; i < array.count; i++) {
        switch(array.array[i].discriminant) {
            case obj_t_string:
                ret = append(ret, "\"");
                ret = append(ret, array.array[i].str);
                ret = append(ret, "\"");
            break;
            case obj_t_array:
                ret = arrayToJson(array.array[i].arr, ret);
            break;
            case obj_t_obj: 
                ret = objectToJson(array.array[i].obj, ret);
            break;
            default:
                string temp = numberToString(array.array[i].num);
                ret = append(ret, temp);
                destroyString(temp);
            break;
        }
        if(i < array.count - 1) {
            ret = append(ret, ",");
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

// Parses a JSON string (e.g., "example") and returns as `string` type
string parseString(string json, size_t *pos) {
    string result = string("");
    printf("string: %s, pos: %ld\n", json, *pos); 

    if (json.at[*pos] != '"') {
        destroyString(result);
        return stringFromCharPtr(""); 
    }
    
    (*pos)++; 

    while (*pos < stringlen(json)) {
        char c = json.at[*pos];

        if (c == '"') {
            (*pos)++; 
            return result; 
        }

        if (c == '\\') {
            (*pos)++; 
            if (*pos >= stringlen(json)) break; 

            switch (json.at[*pos]) {
                case '"':  append(result, "\""); break;
                case '\\': append(result, "\\"); break;
                case '/':  append(result, "/"); break;
                case 'b':  append(result, "\b"); break;
                case 'f':  append(result, "\f"); break;
                case 'n':  append(result, "\n"); break;
                case 'r':  append(result, "\r"); break;
                case 't':  append(result, "\t"); break;
                default:   
                    destroyString(result);
                    return stringFromCharPtr(""); 
            }
        } else {
            char temp[2] = { c, '\0' };
            append(result, temp);
        }
        (*pos)++; 
    }

    destroyString(result);
    return stringFromCharPtr(""); 
}


number_t parseNumber(string json, size_t *pos) {
    size_t start = *pos;

    // Allow digits, decimal points, and signs
    while (*pos < stringlen(json) && (isdigit(json.at[*pos]) || json.at[*pos] == '.' || json.at[*pos] == '-')) {
        (*pos)++;
    }

    string number_str = sliceFromString(json, start, *pos - start);
    double number_value = atof(number_str.at); // Basic parsing of the number

    destroyString(number_str);

    return (number_t){ .number_discriminant = number_t_double, .as_double = number_value };
}

array_t parseArray(string json, size_t *pos) {
    array_t array = createEmptyArray();
    (*pos)++; // Skip '['
    consumeWhitespace(&json, pos);

    while (*pos < stringlen(json) && json.at[*pos] != ']') {
        obj_t_value_t element = parseValue(json, pos);
        array = insertIntoArray(array, element);

        consumeWhitespace(&json, pos);
        if (json.at[*pos] == ',') {
            (*pos)++;
            consumeWhitespace(&json, pos);
        }
    }
    (*pos)++; // Skip ']'

    return array;
}

obj_t parseObject(string json, size_t *pos) {
    obj_t obj = createEmptyObject();
    (*pos)++; // Skip '{'
    consumeWhitespace(&json, pos);
	
    while (*pos < stringlen(json) && json.at[*pos] != '}') {
	    string key = parseString(json, pos);
	
        consumeWhitespace(&json, pos);
        if (json.at[*pos] != ':') {
            // Handle error
            destroyString(key);
            goto invalid_json;
        }
	    (*pos)++; // Skip ':'

        obj_t_value_t value = parseValue(json, pos);
	    
        obj = insertObjectEntry(obj, key, value);
	
        consumeWhitespace(&json, pos);
        if (json.at[*pos] == ',') {
            (*pos)++;
            consumeWhitespace(&json, pos);
        }
        destroyString(key);
    }
    (*pos)++; // Skip '}'
	printf("got here\n");
    return obj;

invalid_json:
    destroyObject(obj);
    return (obj_t){NULL, NULL, 0, NULL}; // Return invalid object on error, basically null
}

obj_t_value_t parseValue(string json, size_t *pos) {
	printf("%s\n", json);
	printf("%ld\n", *pos);
    while (*pos < stringlen(json) && isspace(json.at[*pos])) (*pos)++; 
	static size_t previous_pos = 0;

	
	if(*pos == previous_pos && *pos != 0) {
		printf("stuck");
		exit(EXIT_FAILURE);
	}
	previous_pos = *pos;
    obj_t_value_t result;

    char c = json.at[*pos];
    if (c == '"') {
        result.discriminant = obj_t_string;
        result.str = parseString(json, pos);
    } else if (c == '{') {
        (*pos)++;
        result.discriminant = obj_t_obj;
        result.obj = parseObject(json, pos); 
    } else if (c == '[') {
        (*pos)++;
        result.discriminant = obj_t_array;
        result.arr = parseArray(json, pos); 
    } else if (isdigit(c) || c == '-' || c == '+') {
        result.discriminant = obj_t_number;
        result.num = parseNumber(json, pos); 
    } else if (!strncmp(&json.at[*pos], "true", 4)) {
        result.discriminant = obj_t_true;
        *pos += 4;
    } else if (!strncmp(&json.at[*pos], "false", 5)) {
        result.discriminant = obj_t_false;
        *pos += 5;
    } else if (!strncmp(&json.at[*pos], "null", 4)) {
        result.discriminant = obj_t_null;
        *pos += 4;
    } else {
        goto error;
    }

    return result;

error:
    result.discriminant = obj_t_null;
    return result;
}

obj_t jsonToObject(string json_string) {
    size_t pos = 0;
    consumeWhitespace(&json_string, &pos);

    if (json_string.at[pos] == '{') {
        return parseObject(json_string, &pos);
    }
    
    return (obj_t){NULL, NULL, 0, NULL}; // basically null
}

obj_t JsonToObject_(string json_string) {
	// somehow this should parse a string and turn it into an object

	size_t string_length = stringlen(json_string);
	size_t remaining_length = string_length;
	while(remaining_length --> 0) {
		// this should be fully implemented
		switch(json_string.at[string_length - remaining_length]) {
			case '{':
				break;
			case '}':
				break;
			case '[':
				break;
			case ']':
				break;
			case '\'':
				break;
			case '"':
				break;
			case ',':
				break;
			case ':':
				break;
			default:
				break;
		}
	}

	invalid_json: 	// the string does not contain a valid json object
					// something like null should be returned as the result
		return (obj_t) {
			NULL, NULL, 0, NULL
		};
	 
}

string objectToJson(obj_t object, string external) {
    string ret;
    if(external.data == NULL) {
        ret =  string("{");
    } else {
        ret = append(external, "{");
    }
    printf("json: there are currently %ld entries\n", object.count);
    for (size_t i = 0; i < object.count; i++) {
        ret = append(ret, "\"");
        ret = append(ret, object.key[i].name);
        ret = append(ret, "\"");
        ret = append(ret, " : ");

        switch(object.value[i].discriminant) {
            case obj_t_string:
                ret = append(ret, "\"");
                ret = append(ret, object.value[i].str);
                ret = append(ret, "\"");
            break;
            case obj_t_array:
                ret = arrayToJson(object.value[i].arr, ret);
            break;
            case obj_t_obj:
                ret = objectToJson(object.value[i].obj, ret);
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

void destroyObject(obj_t object) { 
    for(size_t i = 0; i < object.count; i++) {
        destroyString(object.key[i].name);
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
    
