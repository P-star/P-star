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


/*
#pragma once

#include "value.h"
#include "value_string.h"
#include "operator.h"

#include <string>

using namespace std;

// Unresolved
class wpl_value_unresolved_identifier : public wpl_value_strings<string>, public wpl_value {
	public:
	const char *get_type_name() const { return wpl_typename_unresolved_identifier; };
	int get_precedence() const { return wpl_type_unresolved_identifier_precedence; };
	wpl_value_unresolved_identifier *dereference() {
		runtime_error("wpl_value_unresolved_identifier::dereference(): Not allowed");
	}
	wpl_value_unresolved_identifier *clone() const {
		return new wpl_value_unresolved_identifier(*this); 
	};
	wpl_value_unresolved_identifier *clone_empty() const {
		runtime_error("wpl_value_unresolved_identifier::clone_empty(): Not allowed");
	}
	void set_weak (wpl_value *value) {
		runtime_error("wpl_value_unresolved_identifier::set_weak(): Not allowed");
	}

	wpl_value_unresolved_identifier(const wpl_value_unresolved_identifier &copy) :
		wpl_value_strings(copy) {
	}

	wpl_value_unresolved_identifier(const char *value) {
		wpl_value_holder<string>::value = value;
	}

	~wpl_value_unresolved_identifier() {
#ifdef WPL_DEBUG_DESTRUCTION
		DBG("V (" << this << "): Destructing unresolved identifier\n");
#endif
	}

	int do_operator (
			wpl_namespace_session &nss,
			wpl_value **final_result,
			const struct wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
		) {
		return WPL_OP_UNKNOWN;
	}

	string toString() {
		return value;
	}
};

*/

