/*

-------------------------------------------------------------

Copyright (c) MMXIII Atle Solbakken
atle@goliathdns.no
Copyright (c) MMXIV Sebastian Baginski
sebthestampede@gmail.com

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
#include "operator_return_values.h"

using namespace std;

struct wpl_operator_struct;

class wpl_value_wrapper : public wpl_value {
	private:
	wpl_value *value;
	wpl_value *unsafe_pointer;

	wpl_value *lhs;
	wpl_value *rhs;

	public:
	wpl_value_wrapper () {
		value = NULL;
	}
	wpl_value_wrapper (wpl_value *new_value) :
		value(new_value) {
	}
	~wpl_value_wrapper () {
		if (value) {
			value->suicide();
		}
	}

	int get_precedence() const { return value->get_precedence(); };
	const char *get_type_name() const { return wpl_typename_wrapper; }
	wpl_value *clone() const {
		return value->clone();
	};

	wpl_value *get_unsafe_pointer() {
		return unsafe_pointer;
	}

	wpl_value *dereference() {
		return value->dereference();
	}

	void release() {
		value = NULL;
	}

	void set_weak (wpl_value *new_value) {
		if (value) {
			value->suicide();
		}
		value = new_value->clone();
		unsafe_pointer = new_value;
	}

	int do_operator (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const struct wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
		) {
		value->do_operator(exp_state, final_result, op, lhs, rhs);
	}

	unsigned int toUint() { return value->toUint(); };
	int toInt() { return value->toInt(); };
	float toFloat() { return value->toFloat(); };
	double toDouble() { return value->toDouble(); };
	string toString() const { return value->toString(); };
    char* toVoid(){ return value->toVoid(); }
};

