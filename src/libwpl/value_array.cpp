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
#include "value_pointer.h"	

wpl_value *wpl_value_array::define_if_needed(int index) {
	wpl_value *value = get(index);
	if (value == NULL) {
		value = template_type->new_instance();
		set (index, value);
	}
	return value;
}

void wpl_value_array::push_weak(wpl_value *value) {
	define_if_needed(size())->set_weak(value);
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
		int index = rhs->toInt();
		if (index < 0) {
			throw runtime_error ("Negative index received in array subscription []");
		}

		wpl_value *value = get(index);

		if (!exp_state->empty()) {
			shunting_yard_carrier &next_carrier = exp_state->top();
			if (next_carrier.op == &OP_DEFINED) {
				exp_state->pop();

				wpl_value_bool result(value != NULL);
				return result.do_operator_recursive(exp_state, final_result);
			}
		}

		if (value == NULL) {
			value = template_type->new_instance();
			set (index, value);
		}

		return value->do_operator_recursive(exp_state, final_result);
	}
	else if (op == &OP_INDIRECTION) {
		wpl_value *value = define_if_needed(0);
		return value->do_operator_recursive(exp_state, final_result);
	}
	else if (op == &OP_SAVE_DISCARD) {
		if (!lhs) {
			throw runtime_error("Cannot assign discard chain to array with operator '=>' without any previous operands");
		}

/*		cerr << "LHS in array is " << lhs->toInt() << endl;
		exp_state->push_discard(lhs);

		if (wpl_value *wait_top = exp_state->top_waiting()) {
			exp_state->pop_waiting();
			exp_state->push_discard(wait_top);
		}*/

		/* Push discard chain to array */
		int i;
		for (i = 0; i <= exp_state->get_discard_pos(); i++) {
			define_if_needed(size())->set_weak(exp_state->get_discard()[i]);
		}

		/* Push last element, our LHS */
		define_if_needed(size())->set_weak(lhs);

		wpl_value_int res(i + 1);
		return res.do_operator_recursive(exp_state, final_result);
	}
	else if (op == &OP_DISCARD) {
		ret = discard();
	}
	else if (op == &OP_COUNT) {
		wpl_value_int count(size());
		return count.do_operator_recursive(exp_state, final_result);
	}
	else if (op == &OP_POINTERTO) {
		wpl_value_pointer result(exp_state->get_nss(), container_type, this);
		return result.do_operator_recursive(exp_state, final_result);
	}
	else if (op == &OP_ASSIGN) {
		if (!lhs->set_strong(rhs)) {
			cerr << "While assigning value of type " << rhs->get_type_name() <<
				" to array of type " << get_type_name() << " in operator =:\n";
			throw runtime_error("Incompatible types");
		}
		return do_operator_recursive(exp_state, final_result);
	}

	if (ret & WPL_OP_OK) {
		if (ret & WPL_OP_DISCARD) {
			exp_state->push_discard(result);
		}

		return result->do_operator_recursive(exp_state, final_result);
	}

	return ret;
}

int wpl_value_array::do_operator_recursive (wpl_expression_state *exp_state, wpl_value *final_result) {
	if (exp_state->get_flags() & exp_state->CTX_LOOP) {
		int index = exp_state->get_loop_number();
		int max = size();

		if (max == 0) {
			return WPL_OP_RANGE_COMPLETE | WPL_OP_RANGE_ABORT;
		}

		int index_mod = index % max;

		int ret = 0;

		int orig_size = size();

		wpl_value *value = get(index_mod);
		wpl_value *value_next = get(index_mod+1);

		while (size() > orig_size) {
			pop();
		}

		if (value == NULL) {
			return WPL_OP_RANGE_COMPLETE | WPL_OP_RANGE_ABORT;
		}
		else if (value_next == NULL) {
			ret |= WPL_OP_RANGE_COMPLETE;
		}
		else {
			ret |= WPL_OP_RANGE;
		}

		ret |= value->do_operator_recursive(exp_state, final_result);

		return ret;
	}

	return wpl_value::do_operator_recursive(exp_state, final_result);
}

int wpl_value_array::finalize_expression (wpl_expression_state *exp_state, wpl_value *last_value) {
	/*
	   TODO
	   Set array by discard chain?
	 */

	if ((get_flags() & wpl_value_do_finalize) &&  !set_strong (last_value)) {
		cerr << "While setting final result of type " << get_type_name() <<
			" to array of type " << last_value->get_type_name() << ":\n";
		throw runtime_error("Incompatible types");
	}

	return WPL_OP_OK;
}

bool wpl_value_array::set_strong (wpl_value *value) {
	/* This is important to unmask the auto type */
	value = value->dereference();

	wpl_value_array *value_array =
		dynamic_cast<wpl_value_array*>(value);

	if (!value_array) {
		return false;
	}

	if (get_template() != value_array->get_template()) {
		return false;
	}

	replace(*value_array);

	return true;
}

