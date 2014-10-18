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

#include "operator_types.h"
#include "expression_state.h"
#include "value_bool.h"
#include "state.h"
#include "range.h"
#include "value.h"

bool wpl_range_state::counter_operate (
		wpl_expression_state *exp_state,
		const struct wpl_operator_struct *op,
		wpl_value *counter_target
) {
	wpl_value_bool test;
	test.set_do_finalize();
	if ((counter->do_operator(exp_state, &test, op, counter.get(), counter_target) & WPL_OP_OK) == 0) {
		string msg = string("Operator ") + op->op + " not available for value of type " + 
			counter->get_type_name() + " while running range operator ";
		throw runtime_error (msg);
	}
	return test.toBool();
}

wpl_value *wpl_range_state::clone_counter() {
	return counter->clone();
}

wpl_range::wpl_range (
		wpl_expression_state *exp_state,
		const wpl_operator_struct *op,
		wpl_value *lhs,
		wpl_value *rhs
) :
	dummy_exp_state(exp_state),
	flags(0),
	counter_begin(lhs),
	counter_end(rhs),
	boolean_test(op)
{
	wpl_value_bool test;
	test.set_do_finalize();

	if (op == &OP_RANGE_INCLUSIVE) {
		flags |= INCLUSIVE;
	}
	else if (op == &OP_RANGE_EXCLUSIVE) {
		flags |= EXCLUSIVE;
	}
	else {
		string msg = string("do_range_operator() in wpl_range received non-range operator ") +
			op->name;
		throw runtime_error(msg);
	}

	if ((lhs->do_operator(&dummy_exp_state, &test, &OP_LT, lhs, rhs) & WPL_OP_OK) == 0) {
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
			boolean_test = &OP_LT;
		}
	}
	else if (flags & DECREMENT) {
		if (flags & INCLUSIVE) {
			boolean_test = &OP_GTEQ;
		}
		else {
			boolean_test = &OP_GT;
		}
	}
}

wpl_state *wpl_range::new_state (wpl_state *parent, wpl_namespace_session *nss, wpl_io *io) {
	return new wpl_range_state(parent, nss, io, counter_begin, counter_end);
}

int wpl_range::run (
		wpl_state *state,
		wpl_expression_state *exp_state,
		wpl_value *lhs,
		wpl_value *rhs,
		wpl_value *final_result
) {
	wpl_range_state *range_state = (wpl_range_state*) state;

	wpl_value_bool test;
	test.set_do_finalize();

	int ret = WPL_OP_OK; // Always return OK, or the foreach loop will break
	if (!range_state->is_active()) {
		range_state->reset(counter_begin);
		range_state->set_active(true);

		if (flags & EXCLUSIVE) {
			range_state->counter_operate (
					&dummy_exp_state,
					(flags & INCREMENT ? &OP_INC_SUFFIX : &OP_DEC_SUFFIX),
					rhs
			);

			/* If the two numbers are equal, we've got nothing to do */
			if (!range_state->counter_operate (&dummy_exp_state, boolean_test, rhs)) {
				range_state->set_active(false);

				ret |= WPL_OP_RANGE | WPL_OP_RANGE_ABORT;

				return ret;
			}
		}
	}

	unique_ptr<wpl_value> result(range_state->clone_counter());

	range_state->counter_operate (&dummy_exp_state, flags & INCREMENT ? &OP_INC_SUFFIX : &OP_DEC_SUFFIX, rhs);

	if (!range_state->counter_operate (&dummy_exp_state, boolean_test, rhs)) {
		ret |= WPL_OP_RANGE_COMPLETE;
		range_state->set_active(false);
	}

	ret |= result->do_operator_recursive(exp_state, final_result);
	ret |= WPL_OP_RANGE;
	return ret;
}
