/*

-------------------------------------------------------------

Copyright (c) MMXIV Atle Solbakken
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

#include "value_range.h"
#include "value_bool.h"

bool wpl_value_range::counter_operate(wpl_expression_state *exp_state, const struct wpl_operator_struct *op) {
	wpl_value_bool test;
	test.set_do_finalize();
	if ((counter->do_operator(exp_state, &test, op, counter.get(), counter_target) & WPL_OP_OK) == 0) {
		string msg = string("Operator ") + op->op + " not available for value of type " + 
			counter->get_type_name() + " while running range operator ";
		throw runtime_error (msg);
	}
	return test.toBool();
}

int wpl_value_range::do_range_operator (
		wpl_expression_state *exp_state,
		const struct wpl_operator_struct *op,
		wpl_value *lhs,
		wpl_value *rhs
) {
	wpl_value_bool test;
	test.set_do_finalize();

	if ((flags & INITIALIZED) == 0) {
		counter.reset(lhs->clone());
		counter_target = rhs;

		if (op == &OP_RANGE_INCLUSIVE) {
			flags |= INCLUSIVE;
		}
		else if (op == &OP_RANGE_EXCLUSIVE) {
			flags |= EXCLUSIVE;
		}
		else {
			string msg = string("do_range_operator() in wpl_value_range received non-range operator") +
				get_type_name();
			throw runtime_error(msg);
		}

		if ((lhs->do_operator(exp_state, &test, &OP_LT, lhs, rhs) & WPL_OP_OK) == 0) {
			string msg = string("Operator < not available for value of type ") + 
				lhs->get_type_name() + " while running range operator " + op->op;
			throw runtime_error (msg);
		}

		flags |= (test.toBool() ? INCREMENT : DECREMENT);

		if (flags & INCREMENT) {
			if (flags & INCLUSIVE) {
				boolean_test = &OP_LTEQ;
			}
			else {
				counter_operate (exp_state, &OP_INC_SUFFIX);
				boolean_test = &OP_LT;
			}
		}
		else if (flags & DECREMENT) {
			if (flags & INCLUSIVE) {
				boolean_test = &OP_GTEQ;
			}
			else {
				counter_operate (exp_state, &OP_DEC_SUFFIX);
				boolean_test = &OP_GT;
			}
		}

		flags |= INITIALIZED;
	}
	else {
		counter_operate (exp_state, flags & INCREMENT ? &OP_INC_SUFFIX : &OP_DEC_SUFFIX);
	}

	notify->finalize_expression(exp_state, counter.get());

	int ret = WPL_OP_OK | WPL_OP_RANGE;
	if (!counter_operate (exp_state, boolean_test)) {
		ret |= WPL_OP_RANGE_COMPLETE;
	}

	return ret;
}
