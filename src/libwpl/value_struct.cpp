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

#include "value_struct.h"
#include "value_function_ptr.h"
#include "exception.h"
#include "debug.h"

using namespace std;

void wpl_value_struct::output_json() {
	cout << "{}";
}

bool wpl_value_struct::set_strong (wpl_value *value) {
	value = value->dereference();

	if (!value->isStruct()) {
		return false;
	}

	wpl_value_struct *struct_value =
		static_cast<wpl_value_struct*>(value);

	if (mother_struct != struct_value->get_mother_struct()) {
		return false;
	}

	replace_variables(struct_value);

	return true;
}

int wpl_value_struct::do_operator (
		wpl_expression_state *exp_state,
		wpl_value *final_result, 
		const struct wpl_operator_struct *op,
		wpl_value *lhs,
		wpl_value *rhs
) {
	if (lhs != this) {
		throw runtime_error("Left side of '->' was not this struct instance");
	}

	if (op == &OP_ELEMENT) {
		const char *name = rhs->toString().c_str();

		if (wpl_variable *variable = find_variable(name, WPL_NSS_CTX_OUTSIDE)) {
			return variable->get_value()->do_operator_recursive(exp_state, final_result);
		}
		else if (wpl_function *function = find_function_no_parent(name, WPL_NSS_CTX_OUTSIDE)) {
			wpl_value_function_ptr function_ptr(function, this, exp_state);
			return function_ptr.do_operator_recursive(exp_state, final_result);
		}

		cerr << "In -> operator for struct type '" << mother_struct->get_name() << 
			"' while looking for member '" << name << "':\n";
		throw runtime_error("Could not find struct member");
	}
	else if (op == &OP_ASSIGN) {
		if (!set_strong(rhs)) {
			cerr << "In = operator for struct type '" << mother_struct->get_name() << 
				"' while assigning from value of type '" << rhs->get_type_name() << "':\n";
			throw runtime_error("Incompatible types");
		}
		return rhs->do_operator_recursive(exp_state, final_result);
	}
	else if (op == &OP_DISCARD) {
		exp_state->push_discard(lhs);
		return (WPL_OP_OK|WPL_OP_DISCARD);
	}

	return (WPL_OP_UNKNOWN|WPL_OP_NO_RETURN);
}
