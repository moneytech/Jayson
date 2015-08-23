#include "jayson/jstring.h"

jstring_t*
jstring_create(){
    jstring_t* ret;

    if((ret = malloc(sizeof(jstring_t))) == NULL){
        fprintf(stderr, "[Jayson] Allocation Failed.\n");
        abort();
    }

    ret->data = malloc(sizeof(uint8_t) * 1024);
    ret->size = 0;
    ret->asize = 1024;
    return ret;
}

jstring_t *
jstring_slice(jstring_t * str, size_t offset, size_t len){
    jstring_t * jstr = jstring_create();

    for(size_t i = offset; i < len; i++){
        jstring_putc(jstr, str->data[i]);
    }

    return jstr;
}

jstring_t *
jstring_wrap(const char* str){
    jstring_t* ret = jstring_create();
    jstring_puts(ret, str);
    return ret;
}

const char*
jstring_cstr(jstring_t* str){
    if(str->size < str->asize && str->data[str->size] == 0){
        return (const char*) str->data;
    }

    jstring_grow(str, str->size + 1);
    str->data[str->size] = 0;
    return (const char*) str->data;
}

const char
jstring_getc(jstring_t * str, size_t index){
    if(index > str->size || index < 0){
        return NULL;
    }

    return str->data[index];
}

int
jstring_starts_with(jstring_t * str1, jstring_t * str2){
    if(str1->size >= str2->size){
        for(size_t i = 0; i < str2->size; i++){
            if(str1->data[i] != str2->data[i]){
                return 0;
            }
        }

        return 1;
    }

    return 0;
}

int
jstring_equals(jstring_t * str1, jstring_t * str2){
    if(str1->size != str2->size){
        return 0;
    }

    for(size_t i = 0; i < str2->size; i++){
        if(str1->data[i] != str2->data[i]){
            return 0;
        }
    }

    return 1;
}

void
jstring_free(jstring_t * str){
    if(!str){
        return;
    }

    free(str->data);
    free(str);
}

void
jstring_grow(jstring_t* str, size_t size){
    if(str->asize > size){
        return;
    }

    size_t nasize = str->asize + 1024;
    while(nasize < size){
        nasize += 1024;
    }

    uint8_t* ret;
    if((ret = realloc(str->data, nasize)) == NULL){
        fprintf(stderr, "[Jayson] Reallocation Failed. (%lu)\n", nasize);
        abort();
    }

    str->data = ret;
    str->asize = nasize;
}

void
jstring_put(jstring_t * str, const uint8_t* data, size_t size){
    if(str->size + size > str->asize){
        jstring_grow(str, str->size + size);
    }

    memcpy(str->data + str->size, data, size);
    str->size += size;
}

void
jstring_putb(jstring_t * str, jstring_t * data){
    jstring_put(str, data->data, data->size);
}

void
jstring_putc(jstring_t * str, const char c){
    if(str->size + 1 >= str->asize){
        jstring_grow(str, str->size + 1);
    }

    str->data[str->size] = (uint8_t) c;
    str->size++;
}

void
jstring_puts(jstring_t * str, const char* s){
    jstring_put(str, (const uint8_t*) s, strlen(s));
}

void
jstring_printf(jstring_t * str, const char* fmt, ...){
    if(str->size + 1 >= str->asize){
        jstring_grow(str, str->size + 1);
    }

    va_list args_list;
    int args_len;

    va_start(args_list, fmt);
    args_len = vsnprintf((char*) str->data + str->size, str->asize - str->size, fmt, args_list);
    va_end(args_list);

    if(args_len < 0){
        return;
    }

    if(args_len >= str->asize - str->size){
        jstring_grow(str, str->size + args_len + 1);
        va_start(args_list, fmt);
        args_len = vsnprintf((char*) str->data + str->size, str->asize - str->size, fmt, args_list);
        va_end(args_list);
    }

    if(args_len < 0){
        return;
    }

    str->size += args_len;
}