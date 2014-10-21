/*

-------------------------------------------------------------

Copyright (c) MMXIII Atle Solbakken
atle@goliathdns.no

-------------------------------------------------------------

This file is part of P* (P-star).

P* is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

P* is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with P*.  If not, see <http://www.gnu.org/licenses/>.

-------------------------------------------------------------

*/

#include "matcher.h"
#include "exception.h"

#include <iostream>
#include <cstring>
#include <cstdio>
#include <stdexcept>

wpl_matcher::wpl_matcher(const wpl_matcher_position &pos) {
	load_position(pos);
}

wpl_matcher::wpl_matcher(const char *text, int length, const char *filename) {
	set_text (text, length, filename);
}

wpl_matcher::wpl_matcher () :
	filename()
{
	text = NULL;
	text_rpos = NULL;
	text_max = NULL;
}

wpl_matcher::~wpl_matcher () {
}

#define THROW_MATCHER_EXCEPTION(msg) \
	throw wpl_matcher_exception(msg);

/**
 * @brief Parse a name
 *
 * @param target Buffer to store the name
 */
void wpl_matcher::get_word(char *target) {
	// Get name
	int len = search (WORD, WHITESPACE, false);
	check_varname_length(len);
	if (!(len > 0)) {
		THROW_ELEMENT_EXCEPTION("Expected name")
	}
	get_string(target, len);
}

void wpl_matcher::ignore_whitespace() {
	ignore_string_match (WHITESPACE,0);
}

void wpl_matcher::ignore_blockstart() {
	ignore_string_match (WHITESPACE,0);
	if (!ignore_letter ('{')) {
		ostringstream msg;
		msg << "Syntax error near '" << get_string_pointer()[0] << "'";
		THROW_ELEMENT_EXCEPTION(msg.str());
	}
}

void wpl_matcher::set_text (const char *text, const int len, const char *filename) {
	this->text = text;
	this->text_rpos = text;
	this->text_max = text + len;
	this->filename = filename;
}

void wpl_matcher::check_varname_length (const int len) {
	if (len > WPL_VARNAME_SIZE) {
		char tmp[WPL_VARNAME_SIZE];
		get_string (tmp, WPL_VARNAME_SIZE);
		ostringstream msg;
		msg << "Name '" << tmp << "' exceeds maximum length (" <<
			len << " vs " << WPL_VARNAME_SIZE << ")";
		THROW_MATCHER_EXCEPTION(msg.str())
	}
}

bool wpl_matcher::get_letter (const char letter, const uint32_t ignore) {
	ignore_string_match (ignore, 0);
	return ignore_letter (letter);
}

char wpl_matcher::get_letter (const uint32_t match, const uint32_t ignore) {
	ignore_string_match (ignore, 0);
	return get_letter (match);
}

char wpl_matcher::get_letter (const uint32_t match) {
	if (match_letter (match, text_rpos[0])) {
		char ret = text_rpos[0];
		text_rpos++;
		return ret;
	}
	return '\0';
}

char wpl_matcher::get_letter () {
	char ret = text_rpos[0];
	text_rpos++;
	return ret;
}

bool wpl_matcher::search_letter (const uint32_t match) {
	if (match_letter (match, text_rpos[0])) {
		return true;
	}
	return false;
}

int wpl_matcher::get_linepos() {
	int linepos = 1;
	for (const char *pos = text; pos < text_rpos; pos++) {
		char letter = pos[0];
		if (letter == '\n') {
			linepos++;
		}
	}
	return linepos;
}

int wpl_matcher::get_colpos() {
	int colpos = 1;

	// Count backwards to first \n or start of text
	for (const char *pos = text_rpos; pos > text; pos--) {
		char letter = pos[0];
		if (letter == '\n') {
			break;
		}
		colpos++;
	}
	return colpos;
}

int wpl_matcher::match_utf8(const char *text) {
	int first = (*text & 0xff);
	if (M_UTF8_BEGIN(first)) {
		int chars = 0;
		if (first > 0xfd || first < 0xc0) {
			THROW_MATCHER_EXCEPTION("Invalid UTF8-sequence")
		}
		else if (first >= 0xfc) {
			chars = 6;
		}
		else if (first >= 0xf8) {
			chars = 5;
		}
		else if (first >= 0xf0) {
			chars = 4;
		}
		else if (first >= 0xe0) {
			chars = 3;
		}
		else if (first >= 0xc0) {
			chars = 2;
		}

		for (int i = 1; i < chars; i++) {
			int letter = *(text+i) & 0xff;
			if (letter < 0x80 || letter >= 0xc0) {
				THROW_MATCHER_EXCEPTION("Invalid UTF8-sequence")
			}
		}

		return chars;
	}
	return 0;
}

bool wpl_matcher::match_letter (
	const uint32_t match, const char letter
) {
	if (M_UTF8_BEGIN(letter)) {
		THROW_MATCHER_EXCEPTION("UTF8-characters not allowed here")
	}

	if (
		((match & WHITESPACE) && 	M_WHITESPACE(letter)) ||
		((match & NON_WHITESPACE) && 	M_NON_WHITESPACE(letter)) ||
		((match & NUMBER) && 		M_NUMBER(letter)) ||
		((match & WORD) && 		M_WORD(letter)) ||
		((match & NONCTRL_ASCII) && 	M_NONCTRL_ASCII(letter)) ||
		((match & JSON_NEEDS_ESCAPE) &&	M_JSON_NEEDS_ESCAPE(letter)) ||
		((match & QUOTE) && 		M_QUOTE(letter)) ||
		((match & NON_BACKSLASH) &&	M_NON_BACKSLASH(letter)) ||
		((match & NON_ASTERISK) &&	M_NON_ASTERISK(letter)) ||
		((match & NON_CURLY) &&		M_NON_CURLY(letter)) ||
		((match & NEWLINE) && 		M_NEWLINE(letter)) ||
		((match & NON_SEMICOLON) && 	M_NON_SEMICOLON(letter)) ||
		((match & TEXT) &&	 	M_TEXT(letter)) ||
		((match & NON_NEWLINE_WS) && 	M_NON_WHITESPACE(letter)) ||
		((match & ALL) && 		M_ALL(letter)) ||
		((match & LOWERCASE_WORD) && 	M_LOWERCASE_WORD(letter)) ||
		((match & LOWERCASE_LETTER) && 	M_LOWERCASE_LETTER(letter)) ||
		((match & OPERATOR) &&	 	M_OPERATOR(letter)) ||
		((match & EXPRESSION) && 	M_EXPRESSION(letter)) ||
		((match & CAPITAL_WORD) && 	M_CAPITAL_WORD(letter)) ||
		((match & NONBLOCK_BRACKET) &&	M_NONBLOCK_BRACKET(letter)) ||
		((match & CAPITAL_LETTER) && 	M_CAPITAL_LETTER(letter)) ||
		((match & UNDERSCORE) && 	M_UNDERSCORE(letter)) ||
		((match & HASH) &&	 	M_HASH(letter)) ||
		((match & SEMICOLON) && 	M_SEMICOLON(letter))
	) {
		return true;
	}

    if (M_INVALID_CHAR(letter)) {
		ostringstream msg;
		char buf[32];
		sprintf(buf, "%02x", letter);
		msg << "Invalid charater '" << letter << "'/'" << buf << "'";
		THROW_ELEMENT_EXCEPTION(msg.str())
	}

	return false;
}

int wpl_matcher::search (
	const uint32_t match, const uint32_t ignore, const bool critical_mismatch
) {
	int length = 0;
	int pre_length = 0;

	for (const char *rpos = text_rpos; rpos < text_max; rpos++) {
		char letter = rpos[0];

		if (*rpos == '\0') {
			break;
		}

		int utf8_len = 0;
		if ((match & UTF8) && (utf8_len = match_utf8(rpos))) {
			length += utf8_len;
			rpos += utf8_len - 1;
		}
		else if (!length && (ignore & UTF8) && (utf8_len = match_utf8(rpos))) {
			pre_length += utf8_len;
			rpos += utf8_len - 1;
		}
		else if (match_letter (match, letter)) {
			length++;
		}
		else if (length > 0) {
			break;
		}
		else if (match_letter (ignore, letter)) {
			pre_length++;
		}
		else if (critical_mismatch) {
			ostringstream msg;
			msg << "Unexpected character '" << letter << "'";
			THROW_ELEMENT_EXCEPTION(msg.str())
		}
		else {
			length = 0;
			break;
		}
	}

	text_rpos += pre_length;

	return length;
}

/*
   Get a string and increment position pointer. Store
   result int pointer pointed to by target and return length.
*/
int wpl_matcher::get_string (const char **target, const char *search) {
	int i = 0;
	const char *a = search;
	const char *b = text_rpos;
	for ( ; text_rpos+i<=text_max; a++, b++, i++) {
		if (*a != *b) {
			if (*a == '\0') {
				text_rpos += i;
				return i;
			}
			return 0;
		}
	}
	return 0;
}

/*
*/
/**
 * @brief Get a string and increment position pointer. Throw an exception if we attempt to read outside the text.
 *
 * @param target Storage pointer to the beginning of the result
 * @param len Number of characters to increment the position
 *
 * @return returns the len parameter
 */
int wpl_matcher::get_string (const char **target, int len) {
	if (text_rpos+len > text_max) {
		throw runtime_error("wpl_matcher::get_string attempted out of bounds read");
	}
	*target = text_rpos;
	text_rpos += len;
	return len;
}

int wpl_matcher::get_string (char *target, int len) {
	const char *tmp;
	len = get_string (&tmp, len);
	strncpy (target, tmp, len);
	target[len]= '\0';
	return len;
}

/*
   Get a string and increment position pointer.
*/
int wpl_matcher::get_string (const char **target, const uint32_t match, const uint32_t ignore) {
	int len = search (match, ignore, false);

	if (len > 0) {
		*target = text_rpos;
		text_rpos += len;
	}
	else {
		*target = NULL;
	}

	return len;
}

/*
   Get a string and increment position pointer. Chop off
   end if we attempt to read outside the text, without issuing
   any error.
*/
int wpl_matcher::get_string_unsafe (const char **target, int len) {
	if (text_rpos >= text_max) {
		return -1;
	}
	if (text_rpos+len >= text_max) {
		len = text_max - text_rpos;
	}

	*target = text_rpos;
	text_rpos += len;

	return len;
}

int wpl_matcher::get_string_unsafe (char *target, int len_orig) {
	const char *tmp;
	int len = get_string_unsafe (&tmp, len_orig);
	if (len > 0) {
		strncpy (target, tmp, len);
		target[len]= '\0';
	}
	else {
		strncpy (target, "EOF", len_orig);
	}
	return len;
}

/*
   Search text and increment position pointer but don't return anything
*/
int wpl_matcher::ignore_string_match (const uint32_t match, const uint32_t ignore) {
	const char *dummy;
	return get_string (&dummy, match, ignore);
}

void wpl_matcher::go_to_linestart() {
	while (text_rpos >= text) {
		if (M_NEWLINE(*text_rpos)) {
			text_rpos++;
			return;
		}
		text_rpos--;
	}

	text_rpos = text;
}
