#include <stdio.h>
#include <sys/time.h>
#include "jayson/document.h"
#include "jayson/parser.h"

int main(int argc, char** argv){
    jstring_t* json = jstring_create();
    FILE* file = fopen(argv[1], "r");
    if(!file){
        fprintf(stderr, "Cannot open file: %s\n", argv[1]);
        abort();
    }

    while(!(feof(file) || ferror(file))){
        jstring_grow(json, json->size + 1024);
        json->size += fread(json->data + json->size, 1, 1024, file);
    }

    free(file);
    struct timeval stop, start;
    json_parser_t* parser = json_parser_create(json);
    gettimeofday(&start, NULL);
    json_t* doc = json_parser_parse(parser);
    gettimeofday(&stop, NULL);
    fprintf(stdout, "Parsing Took: %lums\n", stop.tv_usec - start.tv_usec);
    json_parser_free(parser);
    json_free(doc);
    return 0;
}