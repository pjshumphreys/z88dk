// ASM pre-processor - read asm files and send output to stdout
// Copyright (c) Paulo Custodio, 2020
// License: http://www.perlfoundation.org/artistic_license_2_0

#pragma once

#include <stdbool.h>

#include "uthash.h"

// keyword lookup tables
typedef enum keyword_e {
	kwNONE, kwDEFINE,
} keyword_e;

typedef struct keyword_t {
	const char* word;
	keyword_e	id;
	UT_hash_handle hh;
} keyword_t;

// tokens
typedef enum token_e {
#define TOKEN(id, string, ok_at_bol)	id,
#include "tokens.def"
	MAX_TOKEN
} token_e;

typedef struct token_t {
	token_e		id;
	const char*	text;
	int			value;
	const char* filename;
	int			line_num;
	int			column;			// to detect labels at column 1 (=0)
	const keyword_t* keyword;	// points to keyword if tkIDENT and found in keyword table
	struct token_t* prev, *next;
} token_t;


// keywords
const keyword_t* keywords_lookup(const char* word);

// tokens
void init_token_reverse_map(void);
token_t* token_new(token_e id);
void token_free(token_t* tok);
const char* token_text(token_e id);
bool token_at_bool(token_e id);
