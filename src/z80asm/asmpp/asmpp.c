// ASM pre-processor - read asm files and send output to stdout
// Copyright (c) Paulo Custodio, 2020
// License: http://www.perlfoundation.org/artistic_license_2_0

#include "common.h"
#include "errors.h"
#include "scan.h"
#include "tokens.h"
#include "utils.h"

#include "uthash.h"
#include "utlist.h"
#include "utstring.h"

#include <ctype.h>

// parser
void parse_fp(FILE* fp, token_t** ptokens) {
	UT_string* line;
	utstring_new(line);

	while (getline(line, fp)) {
		g_line_num++;
		// add start-line token
		token_t* tok = token_new(tkSTART_LINE);
		tok->text = spool_add(utstring_chomp(line));
		DL_APPEND(*ptokens, tok);

		// add line tokens up-to tkEND_LINE
		scan_tokens(utstring_body(line), ptokens);
	}

	utstring_free(line);
}

void parse_file(const char* asm_file, token_t** ptokens) {
	if (opt_verbose)
		printf("Read %s\n", asm_file);

	FILE* in_fp = fopen(asm_file, "r");				// TODO search include path
													// TODO detect recursive includes
	if (in_fp == NULL)
		error("read file failed: %s", asm_file);
	else {
		const char* old_filename = g_filename;
		int old_line_num = g_line_num;

		g_filename = spool_add(asm_file);
		g_line_num = 0;

		parse_fp(in_fp, ptokens);

		g_filename = old_filename;
		g_line_num = old_line_num;

		fclose(in_fp);
	}
}

// parse lists of tokens
void tokens_init(token_t** ptokens) {
	*ptokens = NULL;
}

void tokens_free(token_t** ptokens) {
	while (*ptokens) {
		token_t* head = *ptokens;
		DL_DELETE(*ptokens, head);
		token_free(head);
	}
}

bool are_next(token_t* tok, token_e id, ...) {		// end with -1
	va_list ap;
	va_start(ap, id);
	while ((int)id >= 0) {
		tok = tok->next;
		if (tok == NULL || tok->id != id)
			return false;
		id = va_arg(ap, token_e);
	}
	va_end(ap);
	return true;
}

void preprocess_tokens(token_t** ptokens) {
	for (token_t* tok = *ptokens; tok != NULL; tok = tok->next) {
	redo:
		if (tok == NULL) tok = *ptokens;		// may have been deleted
		if (tok == NULL) break;					// empty list

		switch (tok->id) {
		case tkSTART_LINE:
			// identify labels
			if (opt_labelcol1 && are_next(tok, tkIDENT, -1) && tok->next->column == 0) {
				tok->next->id = tkLABEL;
				goto redo;
			}
			else if (are_next(tok, tkIDENT, tkCOLON, -1)) {
				tok->next->id = tkLABEL;
				goto redo;
			}
			else if (!opt_dotdirective && are_next(tok, tkDOT, tkIDENT, -1)) {
				tok->next->next->id = tkLABEL;
				// remove dot
				token_t* dot = tok->next;
				DL_DELETE(*ptokens, dot);
				token_free(dot);
				goto redo;
			}

			// remove empty lines
			if (tok->id == tkSTART_LINE && are_next(tok, tkEND_LINE, -1)) {
				token_t* start_line = tok;
				token_t* end_line = tok->next;
				if (tok == start_line) tok = end_line->next;

				// delete start_line and end_line
				DL_DELETE(*ptokens, start_line);
				DL_DELETE(*ptokens, end_line);
				token_free(start_line);
				token_free(end_line);

				goto redo;
			}

			break;

		case tkLABEL:
			// remove colon after label
			if (are_next(tok, tkCOLON, -1)) {
				token_t* colon = tok->next;
				DL_DELETE(*ptokens, colon);
				token_free(colon);
				goto redo;
			}
			break;

		default:; // ignore other tokens
		}
	}
}

bool needs_space(char last, char this, bool ok_at_bol) {
	if (last == '\n' && !ok_at_bol && !isspace(this))
		return true;
	if (isspace(last) || isspace(this))
		return false;
	if (is_ident(last) && is_ident(this))
		return true;
	if (!is_ident(last) && !is_ident(this))
		return true;
	return false;
}

void output(FILE* out, UT_string* str, bool ok_at_bol) {
	static char last = '\n';

	if (utstring_len(str) == 0)
		return;
	if (needs_space(last, (utstring_body(str))[0], ok_at_bol)) {
		fprintf(out, " ");
		last = ' ';
	}
	fprintf(out, "%s", utstring_body(str));
	last = (utstring_body(str))[utstring_len(str) - 1];
	utstring_clear(str);
}

void output_tokens(FILE* out, token_t** ptokens) {
	const char* filename = NULL;
	int line_num = 0;
	UT_string* str;
	utstring_new(str);

	for (token_t* tok = *ptokens; tok != NULL; tok = tok->next) {
		if (tok->id == tkSTART_LINE ||
			filename != tok->filename || line_num != tok->line_num
			) {
			if (opt_verbose) {
				utstring_printf(str, "; %s\n", tok->text);
				output(out, str, true);
				line_num++;
			}
			if (filename != tok->filename || line_num != tok->line_num) {
				if (filename == tok->filename) {
					utstring_printf(str, " LINE %d\n", tok->line_num);
					output(out, str, true);
				}
				else {
					utstring_printf(str, " LINE %d, \"%s\"\n", tok->line_num, tok->filename);
					output(out, str, true);
				}
				filename = tok->filename;
				line_num = tok->line_num;
			}
		}

		switch (tok->id) {
		case tkEND_LINE:
			utstring_printf(str, "\n");
			output(out, str, true);
			line_num++;
			break;
		case tkSTRING:
			for (const char* p = tok->text; *p; p++) {
				if (p != tok->text)
					utstring_printf(str, ",");
				utstring_printf(str, "%d", *(unsigned char*)p);
			}
			output(out, str, false);
			break;
		case tkLABEL:
			utstring_printf(str, "%s:", tok->text);
			output(out, str, true);
			break;
		case tkIDENT:
			utstring_printf(str, "%s", tok->text);
			output(out, str, false);
			break;
		case tkNUMBER:
			utstring_printf(str, "%d", tok->value);
			output(out, str, false);
			break;
		default:
			assert(tok->id < MAX_TOKEN);
			utstring_printf(str, "%s", token_text(tok->id));
			output(out, str, token_at_bool(tok->id));
		}
	}

	utstring_free(str);
}

void preprocess(const char* asm_file) {
	UT_string* i_file;
	utstring_new(i_file);

	replace_ext(i_file, asm_file, ".i");
	if (opt_verbose)
		printf("Preprocess %s to %s\n", asm_file, utstring_body(i_file));

	FILE *out_fp = fopen(utstring_body(i_file), "w");
	if (out_fp == NULL)
		error("write file failed: %s", utstring_body(i_file));
	else {
		token_t* tokens;				// Double-Linked list of all input tokens
		tokens_init(&tokens);			// init empty list
		parse_file(asm_file, &tokens);	// parse a file, may recurse to INCLUDE other files
		preprocess_tokens(&tokens);		// do macro substituition in the tokens list
		output_tokens(out_fp, &tokens);	// output result
		fclose(out_fp);

		tokens_free(&tokens);

		if (g_num_errors)
			remove(utstring_body(i_file));
	}

	utstring_free(i_file);
}

int main(int argc, char* argv[]) {
	init_token_reverse_map();

	// parse command line
	if (argc == 1)
		usage();
	else {
		for (int i = 1; i < argc; i++) {
			if (argv[i][0] == '-') {
				switch (argv[i][1]) {
				case 'a':
					if (strcmp(argv[i], "-atoctal") == 0)
						opt_atoctal = true;
					else
						error_unknown_option(argv[i]);
					break;
				case 'd':
					if (strcmp(argv[i], "-dotdirective") == 0)
						opt_dotdirective = true;
					else
						error_unknown_option(argv[i]);
					break;
				case 'h':
					if (strcmp(argv[i], "-hashhex") == 0)
						opt_hashhex = true;
					else
						error_unknown_option(argv[i]);
					break;
				case 'n':
					if (strcmp(argv[i], "-noprec") == 0)
						opt_noprec = true;
					else
						error_unknown_option(argv[i]);
					break;
				case 'l':
					if (strcmp(argv[i], "-labelcol1") == 0)
						opt_labelcol1 = true;
					else
						error_unknown_option(argv[i]);
					break;
				case 's':
					if (strcmp(argv[i], "-stringbasic") == 0)
						opt_string = stringbasic;
					else if (strcmp(argv[i], "-stringsimple") == 0)
						opt_string = stringsimple;
					else
						error_unknown_option(argv[i]);
					break;
				case 'u':
					if (strcmp(argv[i], "-ucase") == 0)
						opt_ucase = true;
					else
						error_unknown_option(argv[i]);
					break;
				case 'v':
					if (strcmp(argv[i], "-v") == 0)
						opt_verbose = true;
					else
						error_unknown_option(argv[i]);
					break;
				default:
					error_unknown_option(argv[i]);
				}
			}
			else
				preprocess(argv[i]);
		}
	}

	if (g_num_errors)
		return EXIT_FAILURE;
	else
		return EXIT_SUCCESS;
}
