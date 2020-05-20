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

#include "value_env.h"
#include "operator.h"
#include "value_string.h"

int wpl_value_env::do_operator (
		wpl_expression_state *exp_state,
		wpl_value *final_result,
		const wpl_operator_struct *op,
		wpl_value *lhs,
		wpl_value *rhs
) {
	if (op == &OP_ELEMENT || op == &OP_ARRAY_SUBSCRIPTING) {
		const char *str = exp_state->get_io().get_env(rhs->toString().c_str());
		/* Check if next carrier is defined-operator */
		if (!exp_state->empty()) {
			shunting_yard_carrier &next_carrier = exp_state->top();
			if (next_carrier.op == &OP_DEFINED) {
				exp_state->pop();
				wpl_value_bool result(str != NULL);
				return result.do_operator_recursive(exp_state, final_result);
			}
		}

		/* Crash if we attempt to used an undefined environment variable */
		if (!str) {
			cerr << "While searching for environment variable '" <<
				rhs->toString() << "':\n";
			throw runtime_error("Could not find environment variable");
		}

		wpl_value_string result_string(str);
		return result_string.do_operator_recursive(exp_state, final_result);
	}
	return WPL_OP_UNKNOWN;
}
