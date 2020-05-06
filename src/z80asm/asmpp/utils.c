// ASM pre-processor - read asm files and send output to stdout
// Copyright (c) Paulo Custodio, 2020
// License: http://www.perlfoundation.org/artistic_license_2_0

#include "utils.h"
#include <stdbool.h>
#include "uthash.h"

// string pool
typedef struct spool_t {
	char *str;
	UT_hash_handle hh;
} spool_t;

static spool_t *g_spool = NULL;

// memory allocation
static void* check_mem(void* p) {
	if (!p) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}
	return p;
}

void* xmalloc(size_t size) {
	return check_mem(malloc(size));
}

void* xcalloc(size_t count, size_t size) {
	return check_mem(calloc(count, size));
}

char* xstrdup(const char* src) {
	if (src)
		return check_mem(strdup(src));
	else
		return NULL;
}

char* xstrndup(const char* src, size_t len) {
	if (src) {
		char* dst = xmalloc(len + 1);
		strncpy(dst, src, len);
		dst[len] = '\0';
		return dst;
	}
	else
		return NULL;
}

// strings
char *str_toupper(char *str) {
	for (char *p = str; *p; p++)
		*p = toupper(*p);
	return str;
}

char *str_chomp(char* str) {
	char* p = str + strlen(str);
	while (p > str && isspace(p[-1]))
		p--;
	*p = '\0';
	return str;
}

char* utstring_chomp(UT_string* str) {
	str_chomp(utstring_body(str));
	utstring_len(str) = strlen(utstring_body(str));
	return utstring_body(str);
}

// string pool
static void spool_dtor(void) {
	spool_t *elem, *tmp;
	HASH_ITER(hh, g_spool, elem, tmp) {
		HASH_DEL(g_spool, elem);
		free(elem->str);
		free(elem);
	}
}

static void spool_init(void) {
	static bool inited = false;
	if (!inited) {
		inited = true;
		atexit(spool_dtor);
	}
}

const char *spool_add(const char *str) {
	spool_init();
	if (str == NULL) return NULL;		// special case

	spool_t *found;
	HASH_FIND_STR(g_spool, str, found);
	if (found) return found->str;		// found

	found = xcalloc(1, sizeof(spool_t));
	found->str = xstrdup(str);
	HASH_ADD_STR(g_spool, str, found);
	return found->str;
}

const char* spool_addn(const char *str, size_t len) {
	char* copy = xstrndup(str, len);
	const char* ret = spool_add(copy);
	free(copy);
	return ret;
}

// files
void replace_ext(UT_string* out, const char* file, const char* new_ext) {
	const char* p = file + strlen(file);
	while (p > file && p[-1] != '/' && p[-1] != '\\' && p[-1] != '.')
		p--;
	if (p > file && p[-1] == '.')
		p--;
	else
		p = file + strlen(file);		// not do found

	utstring_clear(out);
	utstring_bincpy(out, file, p - file);
	utstring_bincpy(out, new_ext, strlen(new_ext));
}

bool getline(UT_string* out, FILE* fp) {
	utstring_clear(out);
	int c;
	while ((c = getc(fp)) != EOF) {
		if (c == '\r') {
			c = getc(fp);
			if (c != '\n' && c != EOF)
				ungetc(c, fp);
			utstring_bincpy(out, "\n", 1);
			return true;
		}
		else if (c == '\n') {
			utstring_bincpy(out, "\n", 1);
			return true;
		}
		else {
			char buffer = (char)c;
			utstring_bincpy(out, &buffer, 1);
		}
	}
	if (utstring_len(out) > 0) {
		utstring_bincpy(out, "\n", 1);			// add missing newline
		return true;
	}
	else
		return false;
}

