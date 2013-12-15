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

#include <sstream>
#include <string>
#include <stdexcept>

#include "matcher.h" 

#if _MSC_VER
#define snprintf _snprintf
#endif

#define THROW_ELEMENT_EXCEPTION(msg) \
	throw wpl_element_exception(msg, get_position());

using namespace std;

class wpl_runtime_exception : public ostringstream {
	public:
	wpl_runtime_exception (const string &msg) :
		ostringstream(msg)
	{}
	wpl_runtime_exception () {
	}
	wpl_runtime_exception (const wpl_runtime_exception &copy) :
		ostringstream(copy.str())
	{}
};

class wpl_element_exception {
	private:
	const struct wpl_matcher_position pos;
	string text;

	public:
	wpl_element_exception (const struct wpl_matcher_position &_pos) :
		text(),
		pos(_pos)
	{}
	wpl_element_exception (const char *msg, const struct wpl_matcher_position &_pos) :
		text(msg),
		pos(_pos)
	{}
	wpl_element_exception (const string &msg, const struct wpl_matcher_position &_pos) :
		text(msg),
		pos(_pos)
	{}
	wpl_element_exception (const ostringstream &msg, const struct wpl_matcher_position &_pos) :
		text(msg.str()),
		pos(_pos)
	{}
	wpl_element_exception (const wpl_element_exception &copy) :
		text(copy.text),
		pos(copy.pos)
	{}
	const string what() const {
		return text;
	}
	const struct wpl_matcher_position where() const {
		return pos;
	}
};

class wpl_matcher_exception : public runtime_error {
	public:
	wpl_matcher_exception (const string &msg) : runtime_error (msg) {}
};

class wpl_parser_exception : public runtime_error {
	public:
	wpl_parser_exception (const string &msg) : runtime_error (msg) {}
};


