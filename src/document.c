#include "jayson/document.h"

json_t*
json_new_object(){
    json_object_t* jobj;
    if((jobj = malloc(sizeof(json_object_t))) == NULL) {
        return NULL;
    }

    if((jobj->table = malloc(sizeof(json_object_entry_t*) * JSON_OBJECT_SIZE)) == NULL){
        free(jobj);
        return NULL;
    }

    for(size_t i = 0; i < JSON_OBJECT_SIZE; i++){
        jobj->table[i] = NULL;
    }

    jobj->json.type = json_object;
    return &jobj->json;
}

json_t*
json_new_array(){
    json_array_t* array;
    if((array = malloc(sizeof(json_array_t))) == NULL){
        return NULL;
    }

    array->json.type = json_array;
    array->size = 0;
    array->asize = 16;

    if((array->table = malloc(sizeof(json_t*) * 16)) == NULL){
        free(array);
        return NULL;
    }

    return &array->json;
}

json_t*
json_new_string(const char* value){
    json_string_t* string;
    if((string = malloc(sizeof(json_string_t))) == NULL){
        return NULL;
    }

    string->json.type = json_string;
    string->value = jstring_wrap(value);
    return &string->json;
}

json_t*
json_new_integer(const int value){
    json_integer_t* integer;
    if((integer = malloc(sizeof(json_integer_t))) == NULL){
        return NULL;
    }

    integer->json.type = json_integer;
    integer->value = value;
    return &integer->json;
}

json_t*
json_new_real(const double value){
    json_real_t* real;
    if((real = malloc(sizeof(json_real_t))) == NULL){
        return NULL;
    }

    real->json.type = json_real;
    real->value = value;
    return &real->json;
}

static const json_t J_TRUE = { json_true };
static const json_t J_FALSE = { json_false };
static const json_t J_NULL = { json_null };

json_t*
json_new_boolean(const int value){
    return (json_t*)(value ? &J_TRUE : &J_FALSE);
}

json_t*
json_new_null(){
    return (json_t*)(&J_NULL);
}

void
json_free(json_t* json){
    if(json_is_object(json)){
        json_object_t* jobj = json_to_object(json);

        for(size_t i = 0; i < JSON_OBJECT_SIZE; i++){
            if(jobj->table[i]){
                json_object_entry_t* entry = jobj->table[i];
                free(entry->key);
                json_free(entry->value);
                free(entry);
            }
        }

        free(jobj->table);
        free(jobj);
    } else if(json_is_array(json)){
        json_array_t* jarray = json_to_array(json);

        for(size_t i = 0; i < jarray->size; i++){
            json_free(jarray->table[i]);
        }

        free(jarray);
    } else if(json_is_string(json)){
        json_string_t* jstring = json_to_string(json);
        jstring_free(jstring->value);
        free(jstring);
    } else if(json_is_integer(json)){
        json_integer_t* jinteger = json_to_integer(json);
        free(jinteger);
    } else if(json_is_real(json)){
        json_real_t* jreal = json_to_real(json);
        free(jreal);
    }
}

unsigned long int
hash(char *key) {
    unsigned long int h = 0;
    size_t size = strlen(key);
    for(size_t i = 0; i < size; i++){
        h = (h ^ key[i] ^ (key[i] << 1) ^ (key[i] << 8) + (key[i] << (key[i] % 7)));
    }
    return h % (JSON_OBJECT_SIZE - 1);
}

json_object_entry_t*
new_entry(char* key, json_t* value){
    json_object_entry_t* entry;
    if((entry = malloc(sizeof(json_object_entry_t))) == NULL){
        return NULL;
    }

    if((entry->key = strdup(key)) == NULL){
        free(entry);
        return NULL;
    }

    entry->value = value;
    entry->next = NULL;
    return entry;
}

void
json_object_set(json_t* jobj, const char* key, json_t* value){
    json_object_t* object = json_to_object(jobj);
    unsigned long int bin = hash((char*) key);

    json_object_entry_t* next = object->table[bin];
    json_object_entry_t* last = NULL;

    while(next != NULL && next->key != NULL && strcmp(key, next->key) > 0){
        last = next;
        next = next->next;
    }

    if(next != NULL && next->key != NULL && strcmp(key, next->key) == 0){
        json_free(next->value);
        next->value = value;
    } else{
        json_object_entry_t* entry = new_entry((char*) key, value);
        if(next == object->table[bin]){
            entry->next = next;
            object->table[bin] = entry;
        } else if(next == NULL){
            last->next = entry;
        } else{
            entry->next = next;
            last->next = entry;
        }
    }
}

json_t*
json_object_get(json_t* jobj, const char* key){
    json_object_t* object = json_to_object(jobj);
    unsigned long int bin = hash((char*) key);

    json_object_entry_t* entry = object->table[bin];
    while(entry != NULL && entry->key != NULL && strcmp(key, entry->key) > 0){
        entry = entry->next;
    }

    if(entry == NULL || entry->key == NULL || strcmp(key, entry->key) != 0){
        return NULL;
    } else{
        return entry->value;
    }
}

void
json_array_grow(json_array_t* jarray, size_t size){
    if(jarray->asize >= jarray->size){
        return;
    }

    size_t nasize = jarray->asize + 1;
    while(nasize < size){
        nasize += 1;
    }

    json_t** data;
    if((data = realloc(jarray->table, nasize)) == NULL){
        return;
    }

    jarray->table = data;
    jarray->asize = nasize;
}

json_t*
json_array_get(json_t* jarray, size_t index){
    json_array_t* array = json_to_array(jarray);
    if(index >= 0 && index <= array->size){
        return array->table[index];
    } else{
        return NULL;
    }
}

void
json_array_set(json_t* jarray, size_t index, json_t* value){
    json_array_t* array = json_to_array(jarray);
    if(index < array->asize && index > 0){
        array->table[index] = value;
    }
}

void
json_array_add(json_t* jarray, json_t* value){
    json_array_t* array = json_to_array(jarray);
    if(array->size + 1 > array->asize){
        json_array_grow(array, array->size + 1);
    }

    array->table[array->size + 1] = value;
    array->size++;
}

const char*
json_string_value(json_t* jstr){
    json_string_t* string;
    if((string = json_to_string(jstr)) == NULL){
        return NULL;
    }

    return jstring_cstr(string->value);
}

jstring_t*
json_value(json_t* j){
    if(json_is_true(j)){
        return jstring_wrap("true");
    } else if(json_is_false(j)){
        return jstring_wrap("false");
    } else if(json_is_real(j)){
        jstring_t* js = jstring_create();
        jstring_printf(js, "%d", json_to_real(j)->value);
        return js;
    } else if(json_is_integer(j)){
        jstring_t* js = jstring_create();
        jstring_printf(js, "%d", json_to_integer(j)->value);
        return js;
    } else if(json_is_string(j)){
        return json_to_string(j)->value;
    } else{
        return jstring_wrap("<not valid json_t>");
    }
}

int
json_integer_value(json_t* jint){
    return json_to_integer(jint)->value;
}

int
json_boolean_value(json_t* jbool){
    if(json_is_boolean(jbool)){
        if(jbool == &J_TRUE){
            return 1;
        }
    }

    return 0;
}

double
json_real_value(json_t* jreal){
    return json_to_real(jreal)->value;
}