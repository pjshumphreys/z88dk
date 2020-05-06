// ASM pre-processor - read asm files and send output to stdout
// Copyright (c) Paulo Custodio, 2020
// License: http://www.perlfoundation.org/artistic_license_2_0

#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include "utstring.h"

// memory allocation
void* xmalloc(size_t size);
void* xcalloc(size_t count, size_t size);
char* xstrdup(const char* src);
char* xstrndup(const char* src, size_t len);

// strings
char *str_toupper(char *str);
char *str_chomp(char* str);
char* utstring_chomp(UT_string* str);

// string pool
const char *spool_add(const char *str);
const char* spool_addn(const char *str, size_t len);

// files
void replace_ext(UT_string* out, const char* file, const char* new_ext);
bool getline(UT_string* out, FILE* fp);
