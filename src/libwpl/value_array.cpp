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

#include "value_array.h"
#include "value_int.h"
#include "operator_types.h"
	
wpl_value *wpl_value_array::define_if_needed(int index) {
	wpl_value *value = get(index);
	if (value == NULL) {
		value = template_type->new_instance();
		set (index, value);
	}
	return value;
}

int wpl_value_array::array_subscripting() {
	if (!(lhs == this)) {
		throw runtime_error ("Left side of [] was not this array");
	}
	int index = rhs->toInt();
	if (index < 0) {
		throw runtime_error ("Negative index received in array subscription []");
	}

	wpl_value *value = define_if_needed(index);

	result = value;

	return (WPL_OP_OK|WPL_OP_NAME_RESOLVED);
}

int wpl_value_array::do_operator (
		wpl_expression_state *exp_state,
		wpl_value *final_result,
		const struct wpl_operator_struct *op,
		wpl_value *lhs,
		wpl_value *rhs
) { 
	this->lhs = lhs;
	this->rhs = rhs;

	int ret = WPL_OP_UNKNOWN;

	if (op == &OP_REPLACE_DISCARD) {
		/* Empty the array and then run save discard */
		clear();
		op = &OP_SAVE_DISCARD;
	}

	if (op == &OP_ARRAY_SUBSCRIPTING) {
		ret = array_subscripting();
	}
	else if (op == &OP_INDIRECTION) {
		wpl_value *value = define_if_needed(0);
		return value->do_operator_recursive(exp_state, final_result);
	}
	else if (op == &OP_SAVE_DISCARD) {
		if (exp_state->empty_waiting()) {
			throw runtime_error("Cannot assign discard chain to array with operator '=>' without any previous operands");
		}

		if (wpl_value *wait_top = exp_state->top_waiting()) {
			exp_state->pop_waiting();
			exp_state->push_discard(wait_top);
		}

		/* Push discard chain to array */
		int i;
		for (i = 0; i <= exp_state->get_discard_pos(); i++) {
			define_if_needed(size())->set_weak(exp_state->get_discard()[i]);
		}

		wpl_value_int res(i);
		return res.do_operator_recursive(exp_state, final_result);
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

		/*
		   Check if result is NULL, in which some value is not defined yet. If next
		   operator is defined-operator we run it, and if not, we create the value.
		   */
		if (result == NULL) {
			if (!exp_state->empty()) {
				shunting_yard_carrier next_carrier = exp_state->top();
				if (next_carrier.op == &OP_DEFINED) {
					wpl_value_bool result(false);
					exp_state->pop();
					return result.do_operator_recursive(exp_state, final_result);
				}
			}
		}

		return result->do_operator_recursive(exp_state, final_result);
	}

	return ret;
}

bool wpl_value_array::set_strong (wpl_value *value) {
	value = value->dereference();

	/*
	   TODO
	   create copy_to_array(wpl_value_array *target) or something in wpl_value
	   */
	if (!value->isArray()) {
		return false;
	}

	wpl_value_array *value_array =
		static_cast<wpl_value_array*>(value);

	if (get_template() != value_array->get_template()) {
		return false;
	}

	replace(*value_array);

	return true;
}

