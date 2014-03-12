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

#include "value.h"
#include "typenames.h"
#include "type_precedence.h"

using namespace std;

class wpl_value_double_finalizer : public wpl_value {
	private:
	wpl_value *value_a;
	wpl_value *value_b;

	public:
	wpl_value_double_finalizer(wpl_value *value_a, wpl_value *value_b) :
		value_a(value_a),
		value_b(value_b)
	{}
	int get_precedence() const { return wpl_type_precedence_double_finalizer; };
	const char *get_type_name() const { return wpl_typename_double_finalizer; }
	wpl_value *clone() const {
		return new wpl_value_double_finalizer (*this);
	};

	int finalize_expression (wpl_expression_state *exp_state, wpl_value *last_value) override {
		int ret = 0;

		ret |= value_a->finalize_expression(exp_state, last_value);
		ret |= value_b->finalize_expression(exp_state, last_value);

		return ret;
	}
};

