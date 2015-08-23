cmake_minimum_required(VERSION 3.2)
project(Jayson)

include_directories(include)

add_executable(Jayson ${SOURCE_FILES} main.c include/jayson/jstring.h src/jstring.c include/jayson/document.h src/document.c include/jayson/parser.h src/parser.c)