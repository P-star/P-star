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

#pragma once

#include <string>
#include <stdexcept>

#include "matcher.h" 

#if _MSC_VER
#define snprintf _snprintf
#endif

using namespace std;

static const int exception_msg_length = 256;
static char exception_msg[exception_msg_length];
static wpl_matcher_position exception_matcher_position;

class wpl_runtime_exception : public runtime_error {
	public:
	wpl_runtime_exception (const string &msg) : runtime_error (msg) {}
};

class wpl_matcher_exception : public runtime_error {
	public:
	wpl_matcher_exception (const string &msg) : runtime_error (msg) {}
};

class wpl_parser_exception : public runtime_error {
	public:
	wpl_parser_exception (const string &msg) : runtime_error (msg) {}
};

class wpl_element_exception : exception {
	private:
	const char *what_text;
	const struct wpl_matcher_position *where_text;

	public:
	wpl_element_exception (const char *what, const struct wpl_matcher_position *where) {
		this->what_text = what;
		this->where_text = where;
	}
	const char *what() const throw() {
		return what_text;
	}
	const struct wpl_matcher_position *where() const throw() {
		return where_text;
	}
};

#define THROW_RUNTIME_EXCEPTION(msg) \
	throw wpl_runtime_exception(msg);

#define THROW_ELEMENT_EXCEPTION(msg) \
	save_position(&exception_matcher_position); \
	throw wpl_element_exception(msg,&exception_matcher_position);

