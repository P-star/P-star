/*

-------------------------------------------------------------

Copyright (c) MMXIII-MMXIV Atle Solbakken
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

#include "parseable.h"
#include "identifier.h"

class wpl_value;
class wpl_namespace;
class wpl_type_complete;
class wpl_type_user_incomplete;

class wpl_type : public wpl_parseable_identifier {
	private:

	public:
	wpl_type(const char *name) : wpl_parseable_identifier(name) {}
	virtual ~wpl_type() {
#ifdef WPL_DEBUG_DESTRUCTION
		DBG("T (" << (wpl_identifier*)this << "): Destructing type\n");
#endif
	}

	virtual int get_precedence() const = 0;
	virtual void suicide() = 0;

	bool isType() const {
		return true;
	}

	virtual bool isIncomplete() const {
		return false;
	}
	virtual bool isComplete() const {
		return false;
	}
	virtual bool isTemplate() const {
		return false;
	}
};

/**
 * @brief Class for simple types like int, string and array. User-defined types are also considered complete (must parse them first for them to work).
 */
class wpl_type_complete : public wpl_type {
	public:
	wpl_type_complete (const char *name) : wpl_type(name) {}
	virtual ~wpl_type_complete() {
#ifdef WPL_DEBUG_DESTRUCTION
		DBG("T (" << (wpl_identifier*)this << "): Destructing complete type\n");
#endif
	}

	virtual wpl_value *new_instance() const = 0;
	virtual void suicide() = 0;

	void parse_value (wpl_namespace *parent_namespace);

	virtual bool check_type(const wpl_type_complete *type) const {
		return (type == this);
	}
	bool isComplete() const {
		return true;
	}
};

/**
 * @brief Class for types like struct and class
 */
class wpl_type_incomplete : public wpl_type {
	public:
	wpl_type_incomplete (const char *name) : wpl_type(name) {}

	virtual wpl_type_user_incomplete *new_instance(const char *name) const = 0;
	virtual ~wpl_type_incomplete() {
#ifdef WPL_DEBUG_DESTRUCTION
		DBG("T (" << (wpl_identifier*)this << "): Destructing incomplete type\n");
#endif
	}
	virtual void suicide() = 0;
	void parse_value(wpl_namespace *parent_namespace) override;

	bool isIncomplete() const {
		return true;
	}
};
