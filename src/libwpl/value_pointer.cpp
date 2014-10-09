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

#include "operator.h"
#include "value_pointer.h"

int wpl_value_pointer::do_operator (
		wpl_expression_state *exp_state,
		wpl_value *final_result,
		const struct wpl_operator_struct *op,
		wpl_value *lhs,
		wpl_value *rhs
) {
	if (op == &OP_INDIRECTION) {
		return get_value()->do_operator_recursive(exp_state, final_result);
	}
	else if (op == &OP_ASSIGN) {
		set_weak(rhs);
		return WPL_OP_OK;
	}
	else if (op == &OP_DISCARD) {
		return wpl_value::do_operator_discard (exp_state, this, final_result);
	}
	else if (op == &OP_POINTERTO) {
		wpl_value_pointer result(get_type(), this);
		return result.do_operator_recursive(exp_state, final_result);
	}
	return WPL_OP_UNKNOWN;
}

void wpl_value_pointer::set_weak(wpl_value *value) {
	if (value->get_type() != (const wpl_type_complete*) wpl_type_global_pointer) {
		ostringstream msg;
		msg << "Can't assign value of type " <<
			value->get_type_name() <<
			" to pointer. Maybe you forgot '&'?\n";
		throw runtime_error(msg.str());
	}

	wpl_value_pointer *ptr = static_cast<wpl_value_pointer*>(value);
	if (!test_type(ptr->get_template_type())) {
		ostringstream msg;
		msg << "Tried to assign pointer of type " << 
			ptr->get_template_type_name() << 
			" to pointer which expects " <<
			get_template_type_name() << endl;
		throw runtime_error(msg.str());
	}

	set_value(ptr->get_value());
}
