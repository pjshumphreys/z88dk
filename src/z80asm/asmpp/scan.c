// ASM pre-processor - read asm files and send output to stdout
// Copyright (c) Paulo Custodio, 2020
// License: http://www.perlfoundation.org/artistic_license_2_0

#include "scan.h"
#include "common.h"
#include "errors.h"
#include "tokens.h"
#include "utils.h"

#include "utlist.h"
#include "utstring.h"

#include <ctype.h>
#include <stdbool.h>

bool is_ident(char c) {
	return c == '_' || c == '.' || isalnum(c);
}

static bool is_ident_start(char c) {
	return c == '_' || isalpha(c);
}

static bool test_quote(char c) {
	if (c == '\'') return true;
	else return false;
}

static bool is_alt_register(const char* p) {
	switch (tolower(*p++)) {
	case 'a':
		switch (tolower(*p++)) {
		case '\'': return true;
		case 'f': return test_quote(*p);
		default: return false;
		}
	case 'f': return test_quote(*p);
	case 'b':
		switch (tolower(*p++)) {
		case '\'': return true;
		case 'c': return test_quote(*p);
		default: return false;
		}
	case 'c': return test_quote(*p);
	case 'd':
		switch (tolower(*p++)) {
		case '\'': return true;
		case 'e': return test_quote(*p);
		default: return false;
		}
	case 'e': return test_quote(*p);
	case 'h':
		switch (tolower(*p++)) {
		case '\'': return true;
		case 'l': return test_quote(*p);
		default: return false;
		}
	case 'l': return test_quote(*p);
	}

	return false;
}

// scan numbers
static int char_digit(int base, char c) {
	if (base == 2) {
		switch (c) {
		case '-': case '0':	return 0;				// binary or bitmap in string
		case '#': case '1': return 1;
		default:			return -1;				// error
		}
	}
	if (isdigit(c)) return c - '0';					// digits
	if (isalpha(c)) return 10 + toupper(c) - 'A';	// digits >= 'A'
	return -1;										// error
}

static bool scan_int(const char** pp, int base, int max_digits, int* pvalue) {
	bool ok = false;
	const char* limit;
	if (max_digits > 0)
		limit = *pp + max_digits;
	else
		limit = *pp + strlen(*pp);

	*pvalue = 0;
	int digit;
	while (*pp < limit && (digit = char_digit(base, **pp)) >= 0 && digit < base) {
		*pvalue = *pvalue * base + digit;
		(*pp)++;
		ok = true;
	}

	return ok;
}

static void scan_number(const char** pp, token_t* tok) {
	const char* ts = *pp;
	tok->id = tkNUMBER;			// assume it will pass
	char quote;
	int base;

	// try each type in sequence
	switch (*ts) {
	case '#':
		// #def
		*pp = ts + 1;
		if (opt_hashhex && scan_int(pp, 16, 0, &tok->value) && !is_ident(**pp))
			return;
		// default
		*pp = ts + 1;
		tok->id = tkHASH;
		return;
	case '$':
		// $ff
		*pp = ts + 1;
		if (scan_int(pp, 16, 0, &tok->value) && !is_ident(**pp))
			return;
		// default
		*pp = ts + 1;
		tok->id = tkASMPC;
		return;
	case '%':
		// %'-#-#'
		quote = ts[1];
		*pp = ts + 2;
		if ((quote == '"' || quote == '\'') && scan_int(pp, 2, 0, &tok->value) && **pp == quote) {
			(*pp)++;	// skip end-quote
			return;
		}
		// %0101
		*pp = ts + 1;
		if (scan_int(pp, 2, 0, &tok->value) && !is_ident(**pp))
			return;
		// default
		*pp = ts + 1;
		tok->id = tkMOD;
		return;
	case '@':
		// @'-#-#'
		quote = ts[1];
		*pp = ts + 2;
		if (!opt_atoctal && (quote == '"' || quote == '\'') && scan_int(pp, 2, 0, &tok->value) && **pp == quote) {
			(*pp)++;	// skip end-quote
			return;
		}
		// @digits
		*pp = ts + 1;
		base = opt_atoctal ? 8 : 2;
		if (scan_int(pp, base, 0, &tok->value) && !is_ident(**pp))
			return;
		// default
		error_invalid_char(*ts);
		*pp = ts + strlen(ts);
		return;
	case '0':
		*pp = ts + 2;
		if (tolower(ts[1]) == 'b' && scan_int(pp, 2, 0, &tok->value) && !is_ident(**pp))
			return;
		*pp = ts + 2;
		if ((tolower(ts[1]) == 'o' || tolower(ts[1]) == 'q') && scan_int(pp, 8, 0, &tok->value) && !is_ident(**pp))
			return;
		*pp = ts + 2;
		if (tolower(ts[1]) == 'x' && scan_int(pp, 16, 0, &tok->value) && !is_ident(**pp))
			return;
		break;
	}

	*pp = ts;
	if (scan_int(pp, 2, 0, &tok->value) && tolower(**pp) == 'b' && !is_ident((*pp)[1])) {
		(*pp)++;	// skip suffix
		return;
	}
	*pp = ts;
	if (scan_int(pp, 8, 0, &tok->value) && (tolower(**pp) == 'o' || tolower(**pp) == 'q') && !is_ident((*pp)[1])) {
		(*pp)++;	// skip suffix
		return;
	}
	*pp = ts;
	if (scan_int(pp, 10, 0, &tok->value) && tolower(**pp) == 'd' && !is_ident((*pp)[1])) {
		(*pp)++;	// skip suffix
		return;
	}
	*pp = ts;
	if (scan_int(pp, 16, 0, &tok->value) && tolower(**pp) == 'h' && !is_ident((*pp)[1])) {
		(*pp)++;	// skip suffix
		return;
	}
	*pp = ts;
	if (scan_int(pp, 10, 0, &tok->value) && !is_ident(**pp))
		return;

	// failed
	tok->id = tkEND_LINE;
	error_invalid_number(ts);
	*pp = ts + strlen(ts);
}

// scan strings
static void scan_string(const char** pp, token_t* tok, UT_string* str) {
	utstring_clear(str);
	const char* ts = *pp;
	int value;
	char c;

	char quote = *(*pp)++;
	if (quote != '"' && quote != '\'')
		goto fail;

	for (bool end = false; !end;) {
		if (**pp == '\0')
			goto fail;
		else if (**pp == quote) {
			if (opt_string == stringbasic) {
				(*pp)++;
				if (**pp == quote) {		// add a quote
					utstring_bincpy(str, &quote, 1);
					(*pp)++;
				}
				else
					end = true;
			}
			else {
				(*pp)++;
				end = true;
			}
		}
		else if (**pp == '\\') {
			if (opt_string != stringc) {
				utstring_bincpy(str, *pp, 1);	// append char
				(*pp)++;
			}
			else {
				(*pp)++;
				switch (*(*pp)++) {
				case 'a': utstring_bincpy(str, "\a", 1); break;
				case 'b': utstring_bincpy(str, "\b", 1); break;
				case 'e': utstring_bincpy(str, "\x1B", 1); break;
				case 'f': utstring_bincpy(str, "\f", 1); break;
				case 'n': utstring_bincpy(str, "\n", 1); break;
				case 'r': utstring_bincpy(str, "\r", 1); break;
				case 't': utstring_bincpy(str, "\t", 1); break;
				case 'v': utstring_bincpy(str, "\v", 1); break;
				case '\\': utstring_bincpy(str, "\\", 1); break;
				case '\'': utstring_bincpy(str, "'", 1); break;
				case '"': utstring_bincpy(str, "\"", 1); break;
				case '?': utstring_bincpy(str, "?", 1); break;
				case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
					(*pp)--;
					if (!scan_int(pp, 8, 3, &value))
						goto fail;
					c = value & 0xFF;
					utstring_bincpy(str, &c, 1);
					break;
				case 'x':
				case 'X':
					if (!scan_int(pp, 16, 0, &value))
						goto fail;
					c = value & 0xFF;
					utstring_bincpy(str, &c, 1);
					break;
				default:
					goto fail;
				}
			}
		}
		else {
			utstring_bincpy(str, *pp, 1);
			(*pp)++;
		}
	}

	if (utstring_len(str) != 0) {
		tok->id = tkSTRING;
		tok->text = spool_add(utstring_body(str));
		return;
	}

fail:
	error_invalid_string(ts);
	*pp = ts + strlen(ts);
	return;
}

static void scan_quoted_char(const char** pp, token_t* tok, UT_string* str) {
	const char* ts = *pp;
	scan_string(pp, tok, str);
	if (tok->id != tkSTRING)
		return;				// error for invalid string already output
		
	if (strlen(tok->text) == 1) {
		tok->id = tkNUMBER;
		tok->value = *(unsigned char*)tok->text;
		tok->text = NULL;
	}
	else {
		error_invalid_quoted_char(ts);
		*pp = ts + strlen(ts);
	}
}

static void found_ident(const char* ts, const char* te, token_t* tok, UT_string *str) {
	utstring_clear(str);
	utstring_bincpy(str, ts, te - ts);

	tok->id = tkIDENT;
	if (opt_ucase) {
		str_toupper(utstring_body(str));			// convert to upper case
		tok->text = spool_add(utstring_body(str));	// and add
	}
	else {
		tok->text = spool_add(utstring_body(str));	// add
		str_toupper(utstring_body(str));			// and convert to upper case
	}

	tok->keyword = keywords_lookup(utstring_body(str));	// lookup keyword in upper case
}

void scan_tokens(const char *input, token_t** ptokens) {
    const char *p = input;
	UT_string* str;
	utstring_new(str);

	for (bool end = false; *p && !end;) {
		while (isspace(*p)) p++;				// skip spaces
		if (!*p || *p == ';') break;			// no more tokens

		const char* ts = p;						// mark start of token
		token_t* tok = token_new(tkEND_LINE);	// assume end-of-line; id is later replaced
		tok->column = p - input;
		DL_APPEND(*ptokens, tok);

		switch (*p++) {
		case '!':
			switch (*p++) {
			case '=': tok->id = tkNOT_EQ; break;
			default: p--; tok->id = tkNOT; break;
			}
			break;
		case '&':
			switch (*p++) {
			case '&': tok->id = tkLOG_AND; break;
			default: p--; tok->id = tkBIN_AND; break;
			}
			break;
		case '|':
			switch (*p++) {
			case '|': tok->id = tkLOG_OR; break;
			default: p--; tok->id = tkBIN_OR; break;
			}
			break;
		case '^':
			switch (*p++) {
			case '^': tok->id = tkLOG_XOR; break;
			default: p--; tok->id = tkBIN_XOR; break;
			}
			break;
		case '*':
			switch (*p++) {
			case '*': tok->id = tkPOWER; break;
			default: p--; tok->id = tkMULT; break;
			}
			break;
		case '=':
			switch (*p++) {
			case '=': tok->id = tkEQ; break;
			default: p--; tok->id = tkEQ; break;
			}
			break;
		case '<':
			switch (*p++) {
			case '>': tok->id = tkNOT_EQ; break;
			case '=': tok->id = tkLESS_EQ; break;
			case '<': tok->id = tkSHIFT_LEFT; break;
			default: p--; tok->id = tkLESS; break;
			}
			break;
		case '>':
			switch (*p++) {
			case '=': tok->id = tkGREATER_EQ; break;
			case '>': tok->id = tkSHIFT_RIGHT; break;
			default: p--; tok->id = tkGREATER; break;
			}
			break;
		case '(': tok->id = tkLEFT_PAREN; break;
		case ')': tok->id = tkRIGHT_PAREN; break;
		case '[': tok->id = tkLEFT_SQUARE; break;
		case ']': tok->id = tkRIGHT_SQUARE; break;
		case '{': tok->id = tkLEFT_BRACKET; break;
		case '}': tok->id = tkRIGHT_BRACKET; break;
		case '+': tok->id = tkPLUS; break;
		case ',': tok->id = tkCOMMA; break;
		case '-': tok->id = tkMINUS; break;
		case '/': tok->id = tkDIV; break;
		case ':': tok->id = tkCOLON; break;
		case '?': tok->id = tkQUESTION; break;
		case '~': tok->id = tkINVERT; break;
		case '"': p--; scan_string(&p, tok, str); break;
		case '\'': p--; scan_quoted_char(&p, tok, str); break;
		case '#':case '$':case '%':case '@':
		case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case'9':
			p--; scan_number(&p, tok); break;
		case '.':
			if (opt_dotdirective && is_ident_start(*p)) {
				while (is_ident(*p))
					p++;
				found_ident(ts, p, tok, str);
			}
			else {
				p = ts + 1;
				tok->id = tkDOT;
			}
			break;
		case '\\':
			input = p;		// start new logical line
			tok = token_new(tkSTART_LINE);
			tok->column = p - input;
			DL_APPEND(*ptokens, tok);
			continue;
		default:
			p--;
			if (is_ident_start(*p)) {
				while (is_ident(*p))
					p++;
				if (*p == '\'' && is_alt_register(ts))		// alternate register a' 
					p++;
				found_ident(ts, p, tok, str);
			}
			else {
				p += strlen(p);
				error_invalid_char(*ts);
			}
		}
	}

	// end end-of-line
	token_t* tok = token_new(tkEND_LINE);			// assume end-of-line; id is later replaced
	tok->column = p - input;
	DL_APPEND(*ptokens, tok);

	utstring_free(str);
}
