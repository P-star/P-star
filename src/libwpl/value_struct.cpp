/*

-------------------------------------------------------------

Copyright (c) MMXIII-MMXIX Atle Solbakken
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
#include "value_void.h"
#include "value_pointer.h"
#include "value_function_ptr.h"
#include "exception.h"
#include "debug.h"


#include "function.h"

using namespace std;

void wpl_value_struct::output_json(wpl_io &io) {
	io << "{";

	vector<wpl_variable*> list;

	wpl_namespace_session::variable_list(list);

	bool first = true;
	for (wpl_variable *var : list) {
		const char *var_name = var->get_name();
		wpl_value *value = var->get_value();

		if (!first) {
			io << ",";
		}

		io << "\"" << var_name << "\":";

		if (value->isArray() || value->isStruct() || value->isPointer()) {
			value->output_json(io);	
		}
		else {
			io << "\"";
			value->output_json(io);	
			io << "\"";
		}

		first = false;
	}
	
	io << "}\n";
}

bool wpl_value_struct::set_strong (wpl_value *value) {
	/* This is important to unmask the auto-type */
	value = value->dereference();

	wpl_value_struct *struct_value =
		dynamic_cast<wpl_value_struct*>(value);

	if (!struct_value) {
		return false;
	}

	if (mother_struct != struct_value->get_mother_struct()) {
		return false;
	}

	replace_variables(struct_value);

	return true;
}

int wpl_value_struct::do_operator_recursive (
		wpl_expression_state *exp_state,
		wpl_value *final_result
) {
	// Expect call to constructor on first run
	if (first_run) {
		first_run = false;

		// Check if we should drop the construction part
		if (!exp_state->empty() && (exp_state->top().op == &OP_INDIRECTION)) {
			exp_state->pop();
		}
		else {
			wpl_function *function = find_function_no_parent(mother_struct->get_name(), WPL_NSS_CTX_SELF);

			// *function may be NULL
			wpl_value_function_ptr function_ptr(function, this, exp_state);
			int ret = function_ptr.do_operator_recursive(exp_state, final_result);
			if (!(ret & WPL_OP_FUNCTION_DID_RUN) && function) {
				ostringstream msg;
				msg << "Could not find default empty constructor () of struct '" <<
					mother_struct->get_name() << "'\n";
				throw runtime_error(msg.str());
			}
			return ret;
		}
	}

	return wpl_value::do_operator_recursive(exp_state, final_result);
}

int wpl_value_struct::do_operator (
		wpl_expression_state *exp_state,
		wpl_value *final_result, 
		const struct wpl_operator_struct *op,
		wpl_value *lhs,
		wpl_value *rhs
) {
	if (first_run) {
		/*
		   If we en up here, the only possibility is that we came through fastop
		   and the operator is the indirection operator * which tells us not to
		   construct. Check if this is true.
		   */
		if (!exp_state->empty() || (op != &OP_INDIRECTION)) {
			throw runtime_error("Struct construction problem (BUG)");
		}

		first_run = false;

		return WPL_OP_OK;
	}

	if (op == &OP_INDIRECTION) {
		/* Indirection tells us to not call constructor. If there are more
		   stuff in the expression, this is an error. */
		if (!exp_state->empty()) {
			throw runtime_error("Inderection operator * for structs must be alone in the statement. Maybe you forgot some parantheses?");
		}
		return WPL_OP_OK;
	}
	else if (op == &OP_ELEMENT) {
		const char *name = rhs->toString().c_str();

		if (wpl_variable *variable = find_variable(name, WPL_NSS_CTX_OUTSIDE)) {
			return variable->get_value()->do_operator_recursive(exp_state, final_result);
		}
		else if (wpl_function *function = find_function_no_parent(name, WPL_NSS_CTX_OUTSIDE)) {
			wpl_value_function_ptr function_ptr(function, this, exp_state);
//			cerr << "V (" << this << ") new function ptr (" << &function_ptr << ") to function '" << function->get_name() << "'\n";
			return function_ptr.do_operator_recursive(exp_state, final_result); 
		}

		ostringstream msg;
		msg << "In -> operator for struct type '" << mother_struct->get_name() << 
			"' while looking for member '" << name << "': Could not find struct member\n";
		throw runtime_error(msg.str());
	}
	else if (op == &OP_ASSIGN) {
		if (!lhs->set_strong(rhs)) {
			ostringstream msg;
			msg << "In = operator for struct type '" << mother_struct->get_name() << 
				"' while assigning to value of type '" << lhs->get_type_name() << "': Incompatible types\n";
			throw runtime_error(msg.str());
		}
		return rhs->do_operator_recursive(exp_state, final_result);
	}
	else if (op == &OP_DISCARD) {
		return do_operator_discard(exp_state, this, final_result);
	}
	else if (op == &OP_POINTERTO) {
		wpl_value_pointer result(exp_state->get_nss(), mother_struct, this);
		return result.do_operator_recursive(exp_state, final_result);
	}
	else if (op == &OP_FUNCTION_CALL) {
		throw runtime_error("Constructor for structs can only be called at first run");
	}

	return (WPL_OP_UNKNOWN|WPL_OP_NO_RETURN);
}

void wpl_value_struct::notify_destructor(wpl_state *state, wpl_namespace_session *nss, wpl_io &io) {
	wpl_function *function = mother_struct->get_dtor();

	if (!function) {
		return;
	}

	wpl_value_void ret;
	unique_ptr<wpl_state> new_state(function->new_state(state, this, &io));
	function->run(new_state.get(), &ret);
}
