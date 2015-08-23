#include <stdlib.h>
#include "jayson/jstring.h"
#include "jayson/parser.h"

json_parser_t*
json_parser_create(jstring_t* str){
    json_parser_t* parser;
    if((parser = malloc(sizeof(json_parser_t))) == NULL){
        return NULL;
    }

    parser->content = str;
    parser->pos = 0;
    return parser;
}

int
iswhitespace(char c){
    switch(c){
        case '\n':
        case '\t':
        case '\r':
        case ' ':{
            return 1;
        }
        default:{
            return 0;
        }
    }
}

const char
json_parser_peek(json_parser_t* parser){
    return jstring_getc(parser->content, parser->pos);
}

const char
json_parser_next(json_parser_t* parser){
    if(parser->pos >= parser->content->size){
        return '\0';
    }

    char next = jstring_getc(parser->content, parser->pos);
    parser->pos++;
    return next;
}

const char
json_parser_next_real(json_parser_t* parser){
    char next;
    while(iswhitespace(next = json_parser_next(parser)));
    return next;
}

jstring_t*
json_parser_name(json_parser_t* parser){
    jstring_t* name = jstring_create();
    char next;
    while((next = json_parser_next(parser)) != '"'){
        jstring_putc(name, next);
    }
    return name;
}

json_t*
json_parser_string(json_parser_t* parser){
    jstring_t* buffer = jstring_create();

    char next;
    while((next = json_parser_next(parser)) != '"'){
        switch(next){
            case '\\':{
                switch(next = json_parser_next(parser)){
                    case '\\':{
                        jstring_putc(buffer, '\\');
                        break;
                    }
                    case 't':{
                        jstring_putc(buffer, '\t');
                        break;
                    }
                    case '/':{
                        break;
                    }
                    default:{
                        jstring_putc(buffer, next);
                    }
                }
            }
            default:{
                jstring_putc(buffer, next);
            }
        }
    }

    json_t* ret = json_new_string(jstring_cstr(buffer));
    jstring_free(buffer);
    return ret;
}

json_t*
json_parser_number(json_parser_t* parser, char next){
    jstring_t* buffer = jstring_create();
    jstring_putc(buffer, next);
    int real = 0;
    while(((next = json_parser_peek(parser)) >= '0' && next <= '9') || (next == '.' && !real)){
        if(next == '.'){
            real = 1;
        }

        jstring_putc(buffer, next);
        json_parser_next(parser);
    }
    json_t* ret = (real ? json_new_real(atof(jstring_cstr(buffer))) : json_new_integer(atoi(jstring_cstr(buffer))));
    jstring_free(buffer);
    return ret;
}

void
json_parser_skip(json_parser_t* parser, size_t len){
    for(size_t i = 0; i < len; i++){
        json_parser_next(parser);
    }
}

json_t*
json_parser_null(json_parser_t* parser){
    json_parser_skip(parser, 3);
    return json_new_null();
}

json_t*
json_parser_boolean(json_parser_t* parser, char next){
    switch(next){
        case 't':{
            json_parser_skip(parser, 3);
            return json_new_boolean(1);
        }
        case 'f':{
            json_parser_skip(parser, 4);
            return json_new_boolean(0);
        }
        default:{
            return json_new_boolean(0);
        }
    }
}

// Declare it here so you can use it in json_parser_array
json_t* json_parser_object(json_parser_t* parser);

json_t*
json_parser_array(json_parser_t* parser){
    json_t* array = json_new_array();

    char next;
    while((next = json_parser_next_real(parser)) != '\0'){
        if(next == '"'){
            json_array_add(array, json_parser_string(parser));
        } else if(next >= '0' && next <= '9'){
            json_array_add(array, json_parser_number(parser, next));
        } else if(next == 't' || next == 'f'){
            json_array_add(array, json_parser_boolean(parser, next));
        } else if(next == 'n'){
            json_array_add(array, json_parser_null(parser));
        } else if(next == '['){
            json_array_add(array, json_parser_array(parser));
        } else if(next == '{'){
            json_array_add(array, json_parser_object(parser));
        } else if(next == ','){
            // Fallthrough
        } else if(next == ']'){
            break;
        }
    }

    return array;
}

// Define it here though
json_t*
json_parser_object(json_parser_t* parser){
    json_t* obj = json_new_object();
    jstring_t* name = NULL;

    char next;
    while((next = json_parser_next_real(parser)) != '}'){
        switch(next){
            case '"':{
                name = json_parser_name(parser);
                next = json_parser_next_real(parser);
                break;
            }
            case ',':
            case '{':{
                continue;
            }
            case ':':{
                next = json_parser_next_real(parser);
                break;
            }
            default:{
                break;
            }
        }

        if(next == ':'){
            next = json_parser_next_real(parser);
        }

        if(next == '"'){
            json_object_set(obj, jstring_cstr(name), json_parser_string(parser));
        } else if(next >= '0' && next <= '9'){
            json_object_set(obj, jstring_cstr(name), json_parser_number(parser, next));
        } else if(next == 'n'){
            json_object_set(obj, jstring_cstr(name), json_parser_null(parser));
        } else if(next == 't' || next == 'f'){
            json_object_set(obj, jstring_cstr(name), json_parser_boolean(parser, next));
        } else if(next == '{'){
            json_object_set(obj, jstring_cstr(name), json_parser_object(parser));
        } else if(next == '['){
            json_object_set(obj, jstring_cstr(name), json_parser_array(parser));
        }
    }

    return obj;
}

json_t*
json_parser_parse(json_parser_t* parser){
    switch(json_parser_peek(parser)){
        case '{': return json_parser_object(parser);
        case '[': return json_parser_array(parser);
        default: return NULL;
    }
}

void
json_parser_free(json_parser_t* parser){
    if(!parser){
        return;
    }

    jstring_free(parser->content);
    free(parser);
}