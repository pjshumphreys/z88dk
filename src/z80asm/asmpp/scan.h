// ASM pre-processor - read asm files and send output to stdout
// Copyright (c) Paulo Custodio, 2020
// License: http://www.perlfoundation.org/artistic_license_2_0

#pragma once

#include "tokens.h"

bool is_ident(char c);
void scan_tokens(const char *input, token_t** ptokens);
