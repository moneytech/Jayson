#ifndef JAYSON_DOCUMENT_H
#define JAYSON_DOCUMENT_H

#include <stdlib.h>
#include <string.h>
#include "jstring.h"

#ifdef __cplusplus
extern "C"{
#endif

#ifndef JSON_OBJECT_SIZE
#define JSON_OBJECT_SIZE 1024
#endif

#ifndef JSON_ARRAY_SIZE
#define JSON_ARRAY_SIZE 16
#endif

#ifndef __attribute__
#define __attribute__(x)
#endif

typedef enum{
    json_object,
    json_array,
    json_string,
    json_integer,
    json_real,
    json_true,
    json_false,
    json_null
} json_type;

typedef struct{
    json_type type;
} json_t;

#define json_typeof(json) ((json)->type)
#define json_is_object(json) ((json) && json_typeof(json) == json_object)
#define json_is_array(json) ((json) && json_typeof(json) == json_array)
#define json_is_string(json) ((json) && json_typeof(json) == json_string)
#define json_is_integer(json) ((json) && json_typeof(json) == json_integer)
#define json_is_real(json) ((json) && json_typeof(json) == json_real)
#define json_is_true(json) ((json) && json_typeof(json) == json_true)
#define json_is_false(json) ((json) && json_typeof(json) == json_false)
#define json_is_null(json) ((json) && json_typeof(json) == json_null)
#define json_is_number(json) (json_is_integer(json) || json_is_real(json))
#define json_is_boolean(json) (json_is_true(json) || json_is_false(json))

json_t* json_new_object() __attribute__((malloc));
json_t* json_new_array() __attribute__((malloc));
json_t* json_new_string(const char* value) __attribute__((malloc));
json_t* json_new_integer(const int value) __attribute__((malloc));
json_t* json_new_real(const double value) __attribute__((malloc));
json_t* json_new_boolean(const int value) __attribute__((malloc));
json_t* json_new_null() __attribute__((malloc));

void json_free(json_t* json);

typedef struct _json_object_entry_t{
    char* key;
    json_t* value;
    struct _json_object_entry_t* next;
} json_object_entry_t;

typedef struct{
    json_t json;
    json_object_entry_t** table;
} json_object_t;

typedef struct{
    json_t json;
    json_t** table;
    size_t size;
    size_t asize;
} json_array_t;

typedef struct{
    json_t json;
    jstring_t* value;
} json_string_t;

typedef struct{
    json_t json;
    double value;
} json_real_t;

typedef struct{
    json_t json;
    int value;
} json_integer_t;

#ifndef container_of
#define container_of(ptr_, type_, member_)({ \
    const typeof(((type_*) 0)->member_)* __mptr = ((void*) ptr_); \
    (type_*)((char*) __mptr - offsetof(type_, member_)); \
})
#endif

#define json_to_object(json_) container_of(json_, json_object_t, json)
#define json_to_array(json_) container_of(json_, json_array_t, json)
#define json_to_string(json_) container_of(json_, json_string_t, json)
#define json_to_real(json_) container_of(json_, json_real_t, json)
#define json_to_integer(json_) container_of(json_, json_integer_t, json)

void json_object_set(json_t* jobj, const char* key, json_t* value);
json_t* json_object_get(json_t* jobj, const char* key);

json_t* json_array_get(json_t* jarray, size_t index);
void json_array_set(json_t* jarray, size_t index, json_t* value);
void json_array_add(json_t* jarray, json_t* value);

const char* json_string_value(json_t* jstr);

int json_integer_value(json_t* jint);

double json_real_value(json_t* jreal);

#ifdef __cplusplus
};
#endif

#endif