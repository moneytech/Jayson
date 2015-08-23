#ifndef JAYSON_JSTRING_H
#define JAYSON_JSTRING_H

#include <stdint.h>
#include <stddef.h>

#ifndef __attribute__
#define __attribute__(x)
#endif

#ifndef JSTRING_CHUNK_SIZE
#define JSTRING_CHUNK_SIZE 10
#endif

#ifdef __cplusplus
extern "C"{
#endif

typedef struct{
    uint8_t* data;

    size_t size;
    size_t asize;
} jstring_t;

jstring_t * jstring_create() __attribute__((malloc));
jstring_t * jstring_slice(jstring_t * str, size_t offset, size_t len);
jstring_t * jstring_wrap(const char* str);

const char* jstring_cstr(jstring_t * str);
const char jstring_getc(jstring_t * str, size_t index);

int jstring_starts_with(jstring_t * str1, jstring_t * str2);
int jstring_equals(jstring_t * str1, jstring_t * str2);

void jstring_free(jstring_t * str);
void jstring_grow(jstring_t * str, size_t len);
void jstring_put(jstring_t * str, const uint8_t* data, size_t size);
void jstring_putb(jstring_t * str, jstring_t * data);
void jstring_putc(jstring_t * str, const char c);
void jstring_puts(jstring_t * str, const char* s);
void jstring_printf(jstring_t * str, const char* fmt, ...) __attribute__((format));

#ifdef __cplusplus
};
#endif

#endif