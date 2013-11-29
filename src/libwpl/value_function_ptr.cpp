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

#include "value_function_ptr.h"
#include "function.h"
#include "operator_types.h"

#include <list>

wpl_value_function_ptr::wpl_value_function_ptr (
		wpl_function *function,
		wpl_namespace_session *nss_this,
		wpl_expression_state *exp_state
		) {
	this->function = function;
	this->nss_this = nss_this;
	this->saved_discard_pos = exp_state->get_discard_pos();
}

int wpl_value_function_ptr::do_operator (
		wpl_expression_state *exp_state,
		wpl_value *final_result,
		const struct wpl_operator_struct *op,
		wpl_value *lhs,
		wpl_value *rhs
) {
	if (op != &OP_FUNCTION_CALL) {
		return WPL_OP_UNKNOWN;
	}

	if (rhs == (wpl_value*) this) {
		// We have no arguments
		if (lhs) {
			exp_state->push_waiting(lhs);
		}
	}
	else if (lhs == (wpl_value*) this) {
		exp_state->push_discard(rhs);
	}
	else {
		throw runtime_error("wpl_function_ptr::do_operator(): I am not myself (confused)");
	}

	int my_exp_pos = exp_state->pos() - 1; // -1 because we were just popped of
	int discard_pos = saved_discard_pos;

	return exp_state->run_function (function, my_exp_pos, discard_pos, final_result, nss_this);
}
