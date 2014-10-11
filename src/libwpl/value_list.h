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

#include "value_holder.h"
#include "operator.h"

#include <list>
#include <memory>

using namespace std;

class wpl_value_list : public wpl_value {
	private:
	list<unique_ptr<wpl_value>> values;

	public:
	PRIMITIVE_TYPEATTR(list)
	wpl_value_list () {}
	wpl_value_list (const wpl_value_list &copy) : wpl_value(copy) {
		for (const unique_ptr<wpl_value> &value : copy.values) {
			values.push_back(unique_ptr<wpl_value>(value->clone()));
		}
	}

	wpl_value_list *clone() const override {
		return new wpl_value_list(*this);
	}

	int do_operator (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const struct wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
		) {
		return WPL_OP_UNKNOWN;
	}

	void push (wpl_value *value) {
		values.emplace_back(value);
	}
};
