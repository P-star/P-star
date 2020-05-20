/*

-------------------------------------------------------------

Copyright (c) MMXIV-MMXIX Atle Solbakken
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
#include "expression_state.h"
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
		lhs->set_weak(rhs);

		wpl_value_pointer result(*this);
		return result.do_operator_recursive(exp_state, final_result);
	}
	else if (op == &OP_DISCARD) {
		return wpl_value::do_operator_discard (exp_state, this, final_result);
	}
	else if (op == &OP_POINTERTO) {
		wpl_value_pointer result(exp_state->get_nss(), get_type(), this);
		return result.do_operator_recursive(exp_state, final_result);
	}
	return WPL_OP_UNKNOWN;
}

int wpl_value_pointer::finalize_expression (wpl_expression_state *exp_state, wpl_value *last_value) {
	if (exp_state->get_flags() & exp_state->CTX_LOOP) {
		/*
		   In loops, like foreach, we allow assignment of our template type
		   instead of only other pointers
		   */
		if (last_value->get_type() == template_type) {
			set_value(last_value);
			return WPL_OP_OK;
		}
		else if (last_value->get_type() == container_type) {
			wpl_value_pointer *ptr = static_cast<wpl_value_pointer*>(last_value);
			set_value(ptr->get_value());
			return WPL_OP_OK;
		}

		ostringstream msg;
		msg << "Can't assign value of type " <<
			last_value->get_type()->get_name() <<
			" to pointer which expects " <<
			container_type->get_name();
		throw runtime_error(msg.str());
	}
	else {
		set_weak(last_value);
	}

	return WPL_OP_OK;
}

void wpl_value_pointer::set_weak(wpl_value *value) {
	wpl_value_pointer *ptr_value = dynamic_cast<wpl_value_pointer*>(value);
	if (!ptr_value) {
		ostringstream msg;
		msg << "Can't assign value of type " <<
			value->get_type_name() <<
			" to pointer of type " << get_type_name() << ". Maybe you forgot '&'?\n";
		throw runtime_error(msg.str());
	}

	if (ptr_value->get_type() != container_type) {
		ostringstream msg;
		msg << "Tried to assign pointer of type " << 
			ptr_value->get_type_name() << 
			" to pointer which expects " <<
			get_type_name() << endl;
		throw runtime_error(msg.str());
	}

	set_value(ptr_value->get_value());
}
	

void wpl_value_pointer::output_json(wpl_io &io) {
	wpl_value *value = get_value();

	if (value->isPointer() || value->isStruct() || value->isArray()) {
		value->output_json(io);
	}
	else {
		io << "\"";
		value->output_json(io);
		io << "\"";
	}
}
