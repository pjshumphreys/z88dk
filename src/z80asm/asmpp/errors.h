// ASM pre-processor - read asm files and send output to stdout
// Copyright (c) Paulo Custodio, 2020
// License: http://www.perlfoundation.org/artistic_license_2_0

#pragma once

void usage(void); 
void error(const char* fmt, ...);
void error_unknown_option(const char* arg);
void error_invalid_number(const char* at);
void error_invalid_string(const char* at);
void error_invalid_quoted_char(const char* at);
void error_invalid_char(char c);