#ifndef __PARSER_H
#define __PARSER_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>


size_t trimstring(char* outputbuffer, const char* inputbuffer, size_t bufferlen);
size_t firstword(char* outputbuffer, const char* inputbuffer, size_t bufferlen);
bool isvalidascii(const char* inputbuffer, size_t bufferlen);
int findpipe(const char* inputbuffer, size_t bufferlen);

#endif
