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

#ifndef __WPL_MATCHER_H
  #define __WPL_MATCHER_H

#include <stdint.h>
#include <iostream>
#include <stdexcept>

using namespace std;

#define M_CAPITAL_LETTER(v) \
	(v >= 'A' && v <= 'Z')
#define M_LOWERCASE_LETTER(v) \
	(v >= 'a' && v <= 'z')
#define M_NUMBER(v) \
	(v >= '0' && v <= '9')

#define M_UNDERSCORE(v) \
	(v=='_')
#define M_SEMICOLON(v) \
	(v==';')
#define M_COLON(v) \
	(v==':')
#define M_COMMA(v) \
	(v==',')
#define M_DOT(v) \
	(v=='.')
#define M_HASH(v) \
	(v=='#')
#define M_CURLY_OPEN(v) \
	(v=='{')
#define M_CURLY_CLOSE(v) \
	(v=='}')
#define M_PAR_OPEN(v) \
	(v=='(')
#define M_PAR_CLOSE(v) \
	(v==')')
#define M_SQUARE_OPEN(v) \
	(v=='[')
#define M_SQUARE_CLOSE(v) \
	(v==']')
#define M_VERTICAL_BAR(v) \
	(v=='|')
#define M_TILDE(v) \
	(v=='~')
#define M_AMP(v) \
	(v=='&')
#define M_CARET(v) \
	(v=='^')
#define M_LT(v) \
	(v=='<')
#define M_GT(v) \
	(v=='>')
#define M_EQ(v) \
	(v=='=')
#define M_EXCLAMATION(v) \
	(v=='!')
#define M_ADD(v) \
	(v=='+')
#define M_NEGATIVE(v) \
	(v=='-')
#define M_SUB(v) \
	(v=='-')
#define M_MUL(v) \
	(v=='*')
#define M_ASTERISK(v) \
	(v=='*')
#define M_NON_ASTERISK(v) \
	(v!='*')
#define M_DIV(v) \
	(v=='/')
#define M_SLASH(v) \
	(v=='/')
#define M_QUESTION(v) \
	(v=='?')
#define M_PERCENT(v) \
	(v=='%')
#define M_NON_BACKSLASH(v) \
	(v!='\\')

#define M_OPERATOR(v) (		\
	M_EQ(v)||		\
	M_ADD(v)||		\
	M_DOT(v)||		\
	M_SUB(v)||		\
	M_GT(v)||		\
	M_LT(v)||		\
	M_VERTICAL_BAR(v)||	\
	M_AMP(v)||		\
	M_EXCLAMATION(v)||	\
	M_MUL(v)||		\
	M_DIV(v)||		\
	M_CARET(v)||		\
	M_TILDE(v)|| 		\
	M_COMMA(v)||		\
	M_COLON(v)||		\
	M_QUESTION(v)		\
)

#define M_TAB(v) \
	(v==9)
#define M_SPACE(v) \
	(v==32)
#define M_NEWLINE(v) \
	((v=='\r') || (v=='\n'))

#define M_NON_SEMICOLON(v) \
	(v!=';')
#define M_NONBLOCK_BRACKET(v) \
	(M_PAR_OPEN(v)||M_PAR_CLOSE(v)||M_SQUARE_OPEN(v)||M_SQUARE_CLOSE(v))
#define M_WHITESPACE(v) \
	(M_TAB(v)||M_NEWLINE(v)||M_SPACE(v))
#define M_NON_NEWLINE_WHITESPACE(v) \
	(M_TAB(v)||M_SPACE(v))
#define M_NON_WHITESPACE(v) \
	(!M_WHITESPACE(v))
#define M_NON_CURLY(v) \
	((v!='{') && (v!='}'))
#define M_NON_NEWLINE(v) \
	((v!='\r') && (v!='\n'))

#define M_DOUBLE_QUOTE(v) \
	(v=='"')
#define M_SINGLE_QUOTE(v) \
	(v=='\'')
#define M_QUOTE(v) \
	(M_DOUBLE_QUOTE(v)||M_SINGLE_QUOTE(v))

#define M_TEXT(v) \
	(M_NON_CURLY(v) && (v!='$'))

#define M_JSON_NEEDS_ESCAPE(v) \
	((v=='\n')||(v=='\r')||(v=='\t')||(v=='\\')||(v=='"'))

#define M_EXPRESSION(v) \
	(M_OPERATOR(v)||M_WHITESPACE(v)||M_WORD(v)||M_NUMBER(v)||M_NONBLOCK_BRACKET(v)||M_QUOTE(v))

#define M_INVALID_CHAR(v) \
	((v!=0 && v<=8) || v==11 || v==12 || (v>=14 && v<=31) || v==127 || v>253)

#define M_UTF8_BEGIN(v) \
	(v>127 && v<=253)

#define M_NONCTRL_ASCII(v) \
    (v<=127 && !(M_INVALID_CHAR(v)))

#define M_WORD(v) \
	(M_CAPITAL_LETTER(v) || M_LOWERCASE_LETTER(v) || M_UNDERSCORE(v))
#define M_CAPITAL_WORD(v) \
	(M_CAPITAL_LETTER(v) || M_UNDERSCORE(v))
#define M_LOWERCASE_WORD(v) \
	(M_LOWERCASE_LETTER(v) || M_UNDERSCORE(v))

#define M_QUERY_NOT_ENCODED(v) \
	(M_WORD(v) || M_NUMBER(v) || M_TILDE(v) || M_SUB(v) || M_DOT(v))

#define M_HEX(v) \
	(((v>='a')&&(v<='f'))||((v>='A')&&(v<='F'))||((v>='0')&&(v<='9')))

#define M_OPERATOR_SUB_EQ(v) \
	(v[0] == '-' && v[1]== '=')
#define M_OPERATOR_ADD_EQ(v) \
	(v[0] == '+' && v[1]== '=')
#define M_OPERATOR_EQEQ(v) \
	(v[0] == '=' && v[1] == '=')
#define M_OPERATOR_EQ(v) \
	(v[0] == '=')

#define M_ALL(v) \
	(true)

struct wpl_matcher_position {
	const char *text;
	const char *text_rpos;
	const char *text_max;
	string filename;
};

class wpl_matcher {
	private:
	const char *text;
	const char *text_rpos;
	const char *text_max;
	string filename;

	int match_utf8 (const char *text);
	bool match_letter (uint32_t match, const char letter);

	public:
/*
   TODO 
   Clean this up. Order in a logical fashion, remove unused etc.
   */
	static const uint32_t ALL		= 1 << 0;

	static const uint32_t CAPITAL_LETTER	= 1 << 2;
	static const uint32_t LOWERCASE_LETTER	= 1 << 3;
	static const uint32_t NUMBER		= 1 << 4;
	static const uint32_t UNDERSCORE	= 1 << 5;
	static const uint32_t SEMICOLON		= 1 << 6;
	static const uint32_t NON_CURLY		= 1 << 10;
	static const uint32_t HASH		= 1 << 11;
	static const uint32_t EXPRESSION	= 1 << 13;

	static const uint32_t UTF8		= 1 << 14;
	static const uint32_t NONCTRL_ASCII	= 1 << 15;

	static const uint32_t NON_BACKSLASH	= 1 << 16;
	static const uint32_t NON_ASTERISK	= 1 << 17;
	static const uint32_t JSON_NEEDS_ESCAPE	= 1 << 18;
	static const uint32_t NEWLINE		= 1 << 19;
	static const uint32_t WHITESPACE	= 1 << 20;
	static const uint32_t NON_WHITESPACE	= 1 << 21;
	static const uint32_t NON_SEMICOLON	= 1 << 22;
	static const uint32_t TEXT		= 1 << 23;
	static const uint32_t NON_NEWLINE_WS	= 1 << 24;

	static const uint32_t QUOTE		= 1 << 25;
	static const uint32_t CAPITAL_WORD	= 1 << 26;
	static const uint32_t LOWERCASE_WORD	= 1 << 27;
	static const uint32_t NONBLOCK_BRACKET	= 1 << 28;
	static const uint32_t OPERATOR		= 1 << 29;
	static const uint32_t WORD		= 1 << 30;
	static const uint32_t INVALID		= 1 << 31;

	wpl_matcher(const wpl_matcher_position &pos);
	wpl_matcher(const char *text, int length, const char *filename);
	wpl_matcher();
	~wpl_matcher();

	inline int get_position_diff() {
		return text_rpos - text;
	}

	inline int get_remaining_length (){
		return text_max - text_rpos;
	}

	inline bool at_end() {
		return (text_rpos >= text_max || text_rpos[0] == '\0');
	}

	inline bool ignore_string (const char *search) {
		const char *dummy;
		if (get_string (&dummy, search) > 0) {
			return true;
		}
		return false;
	}

	inline bool ignore_null_char() {
		if (text_rpos[0] == '\0') {
			text_rpos++;
			return true;
		}
		return false;
	}

	inline int ignore_letter (const char letter) {
		if (text_rpos[0] == letter) {
			text_rpos++;
			return 1;
		}
		return 0;
	}

	inline int ignore_many_letters (const char letter) {
		int total = 0;
		while (text_rpos[0] == letter) {
			text_rpos++;
			total++;
		}
		return total;
	}

	inline void ignore_string (const int len) {
		text_rpos += len;
		if (text_rpos > text_max) {
			throw runtime_error("wpl_matcher::ignore_string(int): rpos > max");
		}
	}

	inline void revert_string (const int len) {
		text_rpos -= len;
		if (text_rpos < text) {
			throw runtime_error("wpl_matcher::revert_string(int): rpos < min");
		}
	}

	inline const char *get_string_pointer () {
		return text_rpos;
	}

	inline const char *get_string_end () {
		return text_max;
	}

	void get_word (char *target);
	void ignore_blockstart();
	void ignore_whitespace();
	void set_text (const char *text, const int len, const char *filename);
	void check_varname_length (const int len);

	bool get_letter (const char letter, const uint32_t ignore);
	char get_letter (const uint32_t match, const uint32_t ignore);
	char get_letter (const uint32_t match);
	char get_letter ();
	bool search_letter (const uint32_t match);
	bool search_letter (char letter) {
		return *text_rpos == letter;
	}

	int get_linepos();
	int get_colpos();

	int search (const uint32_t match, const uint32_t ignore, const bool critical_mismatch);

	int get_string (const char **target, const char *search);
	int get_string (const char **target, int len);
	int get_string (char *target, int len);
	int get_string (const char **target, const uint32_t match, const uint32_t ignore);

	int get_string_unsafe (const char **target, int len);
	int get_string_unsafe (char *target, int len);

	int ignore_string_match (const uint32_t match, const uint32_t ignore);

	void go_to_linestart();

	public:
	void load_position (const wpl_matcher_position &pos) {
		text = pos.text;
		text_rpos = pos.text_rpos;
		text_max = pos.text_max;
		filename = pos.filename;
	}

	struct wpl_matcher_position get_position () const {
		struct wpl_matcher_position ret;
		ret.text = text;
		ret.text_rpos = text_rpos;
		ret.text_max = text_max;
		ret.filename = filename;
		return ret;
	}
};

class wpl_smart_load_position {
	wpl_matcher *tgt;
	wpl_matcher *src;

	public:
	wpl_smart_load_position(wpl_matcher *tgt, wpl_matcher *src) :
		tgt(tgt), src(src)
	{}
	~wpl_smart_load_position() {
		tgt->load_position(src->get_position());
	}
};

#endif // __WPL_MATCHER_H
