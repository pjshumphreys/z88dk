// ASM pre-processor - read asm files and send output to stdout
// Copyright (c) Paulo Custodio, 2020
// License: http://www.perlfoundation.org/artistic_license_2_0

#include "common.h"

const char* g_filename;			// in spool
int g_line_num;
int g_num_errors;

bool opt_atoctal;
bool opt_dotdirective;
bool opt_hashhex;
bool opt_noprec;
bool opt_labelcol1;
string_e opt_string = stringc;
bool opt_ucase;
bool opt_verbose;


