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

#include "expression_state.h"
#include "operator_types.h"
#include "function.h"

void wpl_expression_state::optimize() {
	/*
	   Merge operator and value carrier for short
	   expressions, makes i++ etc. faster
	   */
	if (run_stack.size() == 2) {
		// fastop for function calls is not implemented,
		// only variables can be resolved
		if (run_stack[0].op == &OP_FUNCTION_CALL) {
			return;
		}
		// no fastop implemented for return operator
		if (run_stack[0].op == &OP_RETURN_OP) {
			return;
		}
		shunting_yard_carrier ca(run_stack[1].value, run_stack[0].op);
		run_stack.clear();
		run_stack.push(ca);
		return;
	}
}

int wpl_expression_state::run_function (
		wpl_function *function,
		int index,
		int discard_pos,
		wpl_value *final_result,
		wpl_namespace_session *nss_this
		)
{
	if (index >= child_states.size()) {
		cerr << "index: " << index << " child states size " << child_states.size() << endl;
		throw runtime_error("Index out of range in wpl_expression_state");
	}

	if (!child_states[index].validate (nss_this)) {
		child_states[index].set(function->new_state(this, get_nss(), nss_this, &get_io()), nss_this);
	}

	wpl_function_state *function_state =
		(wpl_function_state *) child_states[index].get();

	if (!function_state->set_variables_from_expression(this, discard_pos)) {
		ostringstream msg;
		msg << "While loading arguments from expression into function with signature ";
		msg << "\t'" << function->get_function_name() << "' : Function argument mismatch\n";
		throw runtime_error(msg.str());
	}

	set_discard_pos(discard_pos);

	int ret = function->run(function_state, function_state->get_return_value());
/*	
	XXX OK to remove this test?

	if (!(ret & WPL_OP_OK)) {
		ostringstream msg;
		msg << "While running function with signature ";
		msg << "\t'" << function->get_function_name() << "': No return value from function block, try to add 'return 0;'\n";
		throw runtime_error(msg.str());
	}
*/

	return function_state->get_return_value()->do_operator_recursive(this, final_result);
}

int wpl_expression_state::run_runable_operator(
		wpl_runable_operator *runable,
		int index,
		wpl_value *lhs,
		wpl_value *rhs,
		wpl_value *final_result
) {
	/*
	   Remember, if you get the index from exp_state->pos(), to +1 it because it might be -1
	   */
	if (index >= child_states.size()) {
		throw runtime_error("Index out of range in wpl_expression_state");
	}
	if (!child_states[index].validate(runable)) {
		wpl_state *new_state = runable->new_state(this, get_nss(), &get_io());
		child_states[index].set(new_state, runable);
	}

	return runable->run(child_states[index].get(), this, lhs, rhs, final_result);
}
