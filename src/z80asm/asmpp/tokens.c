// ASM pre-processor - read asm files and send output to stdout
// Copyright (c) Paulo Custodio, 2020
// License: http://www.perlfoundation.org/artistic_license_2_0

#include "tokens.h"

#include "common.h"
#include "utils.h"

#include <stdbool.h>

// keywords data
static keyword_t keywords_table[] = {
	{"DEFINE", kwDEFINE},
	{ NULL },
};

static keyword_t* keywords_hash;

// tokens data
static struct {
	const char* text;
	bool		ok_at_bol;
} token_props[MAX_TOKEN] = {
#define TOKEN(id, string, ok_at_bol)	{string, ok_at_bol},
#include "tokens.def"
};

static token_e token_reverse_map[128];			// map single-char tokens to token id

// keywords
static void init_keywords(void) {
	static bool inited = false;
	if (!inited) {
		for (keyword_t* kw = keywords_table; kw->word != NULL; kw++) {
			HASH_ADD_KEYPTR(hh, keywords_hash, kw->word, strlen(kw->word), kw);
		}
		inited = true;
	}
}

const keyword_t* keywords_lookup(const char* word) {
	init_keywords();

	keyword_t* kw;
	HASH_FIND_STR(keywords_hash, word, kw);
	return kw;
}

// tokens
void init_token_reverse_map(void) {
	memset(token_reverse_map, 0, sizeof(token_reverse_map));
	const char* p;
#define TOKEN(id, string, ok_at_bol)	p = string; if (strlen(p) == 1) { token_reverse_map[*p] = id; }
#include "tokens.def"
}

token_t* token_new(token_e id) {
	token_t* tok = xcalloc(1, sizeof(token_t));
	tok->id = id;
	tok->filename = g_filename;
	tok->line_num = g_line_num;
	return tok;
}

void token_free(token_t* tok) {
	free(tok);
}

const char* token_text(token_e id) {
	if (id < MAX_TOKEN)
		return token_props[id].text;
	else
		return "";
}

bool token_at_bool(token_e id) {
	if (id < MAX_TOKEN)
		return token_props[id].ok_at_bol;
	else
		return false;
}