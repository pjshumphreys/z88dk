// ASM pre-processor - read asm files and send output to stdout
// Copyright (c) Paulo Custodio, 2020
// License: http://www.perlfoundation.org/artistic_license_2_0

#include "errors.h"
#include "common.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void usage(void) {
	fprintf(stderr,
		//-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+
		"Usage: asmpp [options] file.asm...\n"
		"    -atoctal        @ is a octal prefix instead of binary\n"
		"    -dotdirective   . preceeds directives; by default . preceeds labels\n"
		"    -hashhex        # is a hex prefix instead of immediate operator\n"
		"    -noprec         No precedence in expressions\n"
		"    -labelcol1      Identifier at column 1 is a label\n"
		"    -stringbasic    BASIC like strings: \"\" represents a \"; default is C-like\n"
		"    -stringsimple   No escape characters; default is C-like\n"
		"    -ucase          Convert identifiers to upper case\n"
		"    -v              Verbose\n"
	);
}

void error(const char* fmt, ...) {
	bool have_prefix = false;
	if (g_filename) {
		fprintf(stderr, "%s:", g_filename);
		have_prefix = true;
	}
	if (g_line_num) {
		fprintf(stderr, "%d:", g_line_num);
		have_prefix = true;
	}
	if (have_prefix)
		fprintf(stderr, " ");

	fprintf(stderr, "error: ");

	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");

	g_num_errors++;
}

void error_unknown_option(const char* arg) {
	error("unknown option: %s", arg);
	usage();
	exit(EXIT_FAILURE);
}

void error_invalid_number(const char* at) {
	error("invalid number at: %s", at);
}

void error_invalid_string(const char* at) {
	error("invalid string at: %s", at);
}

void error_invalid_quoted_char(const char* at) {
	error("invalid quoted character at: %s", at);
}

void error_invalid_char(char c) {
	error("invalid character: %c", c);
}

