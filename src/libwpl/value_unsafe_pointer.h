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

#include "value.h"
#include "typenames.h"
#include "type_precedence.h"

using namespace std;

class wpl_value_unsafe_pointer : public wpl_value {
	private:
	wpl_value *value;

	wpl_value *lhs;
	wpl_value *rhs;

	public:
	wpl_value_unsafe_pointer() {}
	wpl_value_unsafe_pointer(wpl_value *value) {
		this->value = value;
	}
	int get_precedence() const { return wpl_type_precedence_unsafe_pointer; };
	const char *get_type_name() const { return wpl_typename_unsafe_pointer; }
	wpl_value *clone() const {
		return new wpl_value_unsafe_pointer (*this);
	};

	wpl_value *dereference() {
		return value;
	}

	void set_weak (wpl_value *new_value) {
		value = new_value;
	}

	int finalize_expression (wpl_expression_state *exp_state, wpl_value *last_value) override {
		value = last_value;
		return WPL_OP_OK;
	}
};

