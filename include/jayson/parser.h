#ifndef JAYSON_PARSER_H
#define JAYSON_PARSER_H

#ifdef __cplusplus
extern "C"{
#endif

#include "jstring.h"
#include "document.h"

#ifndef __attribute__
#define __attribute__(x)
#endif

typedef struct{
    jstring_t* content;
    size_t pos;
} json_parser_t;

json_parser_t * json_parser_create(jstring_t* str) __attribute__((malloc));
json_t* json_parser_parse(json_parser_t* parser);
void json_parser_free(json_parser_t* parser);

#ifdef __cplusplus
};
#endif

#endif