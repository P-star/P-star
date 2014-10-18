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

#include <cstring>

using namespace std;

class wpl_identifier {
	private:

	char name[WPL_VARNAME_SIZE+1];

	public:
	bool is_name(const char *name) const;
	void set_name(const char *name, int len);
	void set_name(const char *name);

	wpl_identifier () {
		this->name[0] = '\0';
	}
	wpl_identifier (const char *name) {
		set_name (name);
	}
	wpl_identifier (const wpl_identifier &copy) {
		strncpy(name, copy.name, WPL_VARNAME_SIZE+1);
	}
	virtual ~wpl_identifier() {}

	virtual void suicide() {
		delete this;
	}

	virtual bool isType() const {
		return false;
	}
	virtual bool isFunction() const {
		return false;
	}
	virtual bool isScene() const {
		return false;
	}
	virtual bool isVariable() const {
		return false;
	}
	virtual bool isPragma() const {
		return false;
	}
	const char *get_name() const {
		return name;
	}
};

/**
 * @brief This class is used by variables and functions to tell if they are private, protected or public
 */
class wpl_identifier_access_holder : public wpl_identifier {
	private:
	int access_flags;

	public:
	wpl_identifier_access_holder (const char *name, int access) :
		wpl_identifier(name),
		access_flags(access)
	{}
	virtual ~wpl_identifier_access_holder() {}

	int get_access_flags() {
		return access_flags;
	}
};

/**
 * @brief Use this to register identifiers coming from libraries so that we don't free their memory (they must free it themselves). This is a dummy class which only holds the identifiers name to prevent duplication.
 */
class wpl_identifier_reserved_name : public wpl_identifier {
	public:
	wpl_identifier_reserved_name (const char *name) :
		wpl_identifier(name)
	{}
	virtual ~wpl_identifier_reserved_name() {}
};

