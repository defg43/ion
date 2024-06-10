#include "../include/ion.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

obj_t createEmptyObject() {
    return (obj_t) {
        .key = NULL,
        .value = NULL,
        .count = 0,
        .destructor = NULL,
    };
};

obj_t insertObjectEntry(obj_t object, string key, obj_t_value_t value) {
    obj_t_key_t *key_ = realloc(object.key, sizeof(obj_t_key_t) * (object.count + 1));
    obj_t_value_t *value_ = realloc(object.value, sizeof(obj_t_value_t) * (object.count + 1));
    if(key_ == NULL || value_ == NULL) {
        fprintf(stderr, "failed to allocate memory in insertObjectEntry\n");
        exit(EXIT_FAILURE);
    } 
    object.count++;
    object.key[object.count].name = key; // key is just absorbed here and owned here from now on
    // this should be sourced from the string allocator backend once it is implemented
    object.value[object.count] = value;
    return object;
}

obj_t insertSubobjectEntry(obj_t object, string key, obj_t value) {
    obj_t_key_t *key_ = realloc(object.key, sizeof(obj_t_key_t) * (object.count + 1));
    obj_t *value_ = realloc(object.value, sizeof(obj_t) * (object.count + 1));
    if(key_ == NULL || value_ == NULL) {
        fprintf(stderr, "failed to allocate memory in insertSubobjectEntry\n");
        exit(EXIT_FAILURE);
    } 
    object.count++;
    object.key[object.count].name = key; // key is just absorbed here and owned here from now on
    // this should be sourced from the string allocator backend once it is implemented
    object.value[object.count].discriminant = obj_t_obj;
    object.value[object.count].obj = value;
    return object;
}

obj_t insertArrayEntry(obj_t object, string key, array_t value) {
    obj_t_key_t *key_ = realloc(object.key, sizeof(obj_t_key_t) * (object.count + 1));
    array_t *value_ = realloc(object.value, sizeof(array_t) * (object.count + 1));
    if(key_ == NULL || value_ == NULL) {
        fprintf(stderr, "failed to allocate memory in insertArrayEntry\n");
        exit(EXIT_FAILURE);
    } 
    object.count++;
    object.key[object.count].name = key; // key is just absorbed here and owned here from now on
    // this should be sourced from the string allocator backend once it is implemented
    object.value[object.count].discriminant = obj_t_array;
    object.value[object.count].arr = value;
    return object;
}

obj_t insertNumberEntry(obj_t object, string key, number_t value) {
    obj_t_key_t *key_ = realloc(object.key, sizeof(obj_t_key_t) * (object.count + 1));
    number_t *value_ = realloc(object.value, sizeof(number_t) * (object.count + 1));
    if(key_ == NULL || value_ == NULL) {
        fprintf(stderr, "failed to allocate memory in insertNumberEntry\n");
        exit(EXIT_FAILURE);
    } 
    object.count++;
    object.key[object.count].name = key; // key is just absorbed here and owned here from now on
    // this should be sourced from the string allocator backend once it is implemented
    object.value[object.count].discriminant = obj_t_number;
    object.value[object.count].num = value;
    return object;
}

obj_t insertStringEntry(obj_t object, string key, string value) {
    obj_t_key_t *key_ = realloc(object.key, sizeof(obj_t_key_t) * (object.count + 1));
    string *value_ = realloc(object.value, sizeof(string) * (object.count + 1));
    printf("key: %p\nvalue: %p\n", key, value);
    if(key_ == NULL || value_ == NULL) {
        fprintf(stderr, "failed to allocate memory in insertStringEntry\n");
        exit(EXIT_FAILURE);
    } 
    object.count++;
    object.key[object.count].name = key; // key is just absorbed here and owned here from now on
    // this should be sourced from the string allocator backend once it is implemented
    object.value[object.count].discriminant = obj_t_string;
    object.value[object.count].str = value;
    return object;
}

obj_t insertBoolEntry(obj_t object, string key, bool value) {
    obj_t_key_t *key_ = realloc(object.key, sizeof(obj_t_key_t) * (object.count + 1));
    bool *value_ = realloc(object.value, sizeof(bool) * (object.count + 1));
    if(key_ == NULL || value_ == NULL) {
        fprintf(stderr, "failed to allocate memory in insertBoolEntry\n");
        exit(EXIT_FAILURE);
    } 
    object.count++;
    object.key[object.count].name = key; // key is just absorbed here and owned here from now on
    // this should be sourced from the string allocator backend once it is implemented
    if(value) {
        object.value[object.count].discriminant = obj_t_true;
    } else {
        object.value[object.count].discriminant = obj_t_false;
    }
    return object;
}

obj_t insertNullEntry(obj_t object, string key) {
    obj_t_key_t *key_ = realloc(object.key, sizeof(obj_t_key_t) * (object.count + 1));
    if(key_ == NULL) {
        fprintf(stderr, "failed to allocate memory in insertNullEntry\n");
        exit(EXIT_FAILURE);
    } 
    object.count++;
    object.key[object.count].name = key; // key is just absorbed here and owned here from now on
    // this should be sourced from the string allocator backend once it is implemented
    object.value[object.count].discriminant = obj_t_null;
    return object;
}

string arrayToJson(array_t array) {
    (void) array;
    return string("<an array prbably lives here>");
}

string objectToJson(obj_t object) {
    string ret = string("{");
    for (size_t i = 0; i < object.count; i++) {
        ret = append(ret, "\"");
        ret = append(ret, object.key[i].name);
        ret = append(ret, "\" : ");
        switch(object.value[i].discriminant) {
            case obj_t_string:
                ret = append(ret, object.value[i].str);
                break;
            case obj_t_array:
                ret = append(ret, arrayToJson(object.value[i].arr));
                break;
            case obj_t_obj:
                ret = append(ret, objectToJson(object.value[i].obj));
                break;
            case obj_t_number:
                ret = append(ret, "<number>");
            default:
                break;
        }
        ret = append(ret, ", ");
    }
    ret = append(ret, "}");
    return ret;
}

void destroyObject(obj_t object) { 
    for(size_t i = 0; i < object.count; i++) {
        switch(object.value[i].discriminant) {
            case obj_t_string:
                destroyString(object.value[i].str);
                break;
            case obj_t_array:
                object.value[i].arr.destructor(object.value[i].arr.array);
                break;
            case obj_t_obj:
                destroyObject(object.value[i].obj);
                break;
            default:
                break;
        }
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

void destroyArray(array_t arr) {
    arr.destructor(arr.array);
}

    