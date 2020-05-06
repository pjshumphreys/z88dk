// ASM pre-processor - read asm files and send output to stdout
// Copyright (c) Paulo Custodio, 2020
// License: http://www.perlfoundation.org/artistic_license_2_0

#pragma once

#include <stdbool.h>

typedef enum string_e {
	stringc, stringbasic, stringsimple
} string_e;

extern const char* g_filename;			// in spool
extern int g_line_num;
extern int g_num_errors;

extern bool opt_atoctal;
extern bool opt_dotdirective;
extern bool opt_hashhex;
extern bool opt_noprec;
extern bool opt_labelcol1;
extern string_e opt_string;
extern bool opt_ucase;
extern bool opt_verbose;
