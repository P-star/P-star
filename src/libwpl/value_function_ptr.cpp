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
	this->did_run = false;
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

/*	cerr << "V (" << this << ") function ptr of " << function->get_name() << "\n";
	cerr << "V (" << this << ") lhs is " << lhs << "\n";
	cerr << "V (" << this << ") rhs is " << rhs << "\n";
	cerr << "V (" << this << ") discard length:  " << exp_state->get_discard().size() << "\n";*/

	if (rhs == (wpl_value*) this) {
		// We have no arguments
		if (lhs) {
			exp_state->push_waiting(lhs);
		}
	}
	else if (lhs == (wpl_value*) this) {
//		cerr << "V (" << this << ") push to discard " << rhs->get_type_name() << "\n";
		exp_state->push_discard(rhs);
	}
	else {
		throw runtime_error("wpl_function_ptr::do_operator(): I am not myself (confused)");
	}

	int my_exp_pos = exp_state->pos() + 1; // +1 because we were just popped of
	if (my_exp_pos < 0) {
		ostringstream msg;
		msg << "wpl_value_function_ptr::do_operator(): my_exp_pos was < 0 (" << my_exp_pos << ")";
		throw runtime_error(msg.str());
	}
	int discard_pos = saved_discard_pos;

	// Call to default constructor of struct
	if (function == NULL) {
		int discard_length = exp_state->get_discard().size() - discard_pos - 1;
		if (discard_length > 0) {
			throw runtime_error("Default constructor of struct expects no arguments");
		}

		if (!exp_state->empty()) {
			throw runtime_error("Expression did not end after call to constructor");
		}

		return (WPL_OP_OK | WPL_OP_FUNCTION_DID_RUN);
	}

	return (WPL_OP_FUNCTION_DID_RUN | exp_state->run_function (function, my_exp_pos, discard_pos, final_result, nss_this));
}
