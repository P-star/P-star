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

#include "value_hash.h"
#include "value_int.h"
#include "operator_types.h"

#include <string>

int wpl_value_hash::hash_subscripting() {
	if (!(lhs == this)) {
		throw runtime_error ("Left side of [] was not this hash");
	}
	string key = rhs->toString();

	wpl_value *value = get(key);

	if (value == NULL) {
		value = template_type->new_instance();
		set (key, value);
	}

	result = value;

	return (WPL_OP_OK|WPL_OP_NAME_RESOLVED);
}

int wpl_value_hash::do_operator (
		wpl_expression_state *exp_state,
		wpl_value *final_result,
		const struct wpl_operator_struct *op,
		wpl_value *lhs,
		wpl_value *rhs
) { 
	this->lhs = lhs;
	this->rhs = rhs;

	int ret = WPL_OP_UNKNOWN;

	if (op == &OP_ARRAY_SUBSCRIPTING) {
		ret = hash_subscripting();
	}
	else if (op == &OP_DISCARD) {
		ret = discard();
	}
	else if (op == &OP_COUNT) {
		wpl_value_int count(size());
		return count.do_operator_recursive(exp_state, final_result);
	}

	if (ret & WPL_OP_OK) {
		if (ret & WPL_OP_DISCARD) {
			exp_state->push_discard(result);
		}
		return result->do_operator_recursive(exp_state, final_result);
	}

	return ret;
}

bool wpl_value_hash::set_strong (wpl_value *value) {
	value = value->dereference();

	wpl_value_hash *value_hash =
		dynamic_cast<wpl_value_hash*>(value);

	if (!value_hash) {
		return false;
	}

	if (get_template() != value_hash->get_template()) {
		return false;
	}

	replace(*value_hash);

	return true;
}

wpl_value *wpl_value_hash::define(string &key) {
	wpl_value *value = template_type->new_instance();
	set(key, value);
	return value;
}
