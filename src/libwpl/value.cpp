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

#include "value.h"
#include "value_bool.h"
#include "type_precedence.h"
#include "expression_state.h"
#include "operator.h"

wpl_value_return::~wpl_value_return() {
#ifdef WPL_DEBUG_DESTRUCTION
	DBG("VR (" << this << "): Destructing return value, no delete is " << do_delete << endl);
	DBG("    < Value is " << value << endl);
#endif
	if (value && do_delete) {
		value->suicide();
	}
}

wpl_value *wpl_value_return::iterate() {
	if (!value) {
		return NULL;
	}
	return value->next();
}

void wpl_value_return::set(wpl_value *new_value, int flags) {
	if (value && do_delete) {
		value->suicide();
	}
	do_delete = ((flags & WPL_OP_RETURN_REFERENCE) > 0);
	iterable = ((flags & WPL_OP_RETURN_ITERABLE) > 0);
	value = new_value;
}

int wpl_value_return::run(wpl_value **final_result) {
	if (value) {
		*final_result = value;
		return WPL_OP_OK;
	}
	else {
		*final_result = NULL;
		return WPL_OP_NO_RETURN;
	}
}

wpl_value::~wpl_value() {
#ifdef WPL_DEBUG_DESTRUCTION
	DBG("V (" << this << "): Destructing value\n");
#endif
}

wpl_value *wpl_value::create_bool(bool value) {
	return new wpl_value_bool(value);
}

int wpl_value::do_fastop (
		wpl_expression_state *exp_state,
		wpl_value *final_result,
		const wpl_operator_struct *op
		)
{
//	cout << "V (" << this << "): do_fastop with " << exp_state->get_stack().size() << " on stack (type is " << get_type_name() << ") value is " << toString() << " op is " << op->name << "\n" ;
	if ((op->flags & WPL_OP_F_HAS_BOTH) != WPL_OP_F_HAS_BOTH) {
		return do_operator(exp_state, final_result, op, this, this);
	}
	if (op->flags & WPL_OP_F_OPTIONAL_LHS) {
		return do_operator(exp_state, final_result, op, NULL, this);
	}
	if (op->flags & WPL_OP_F_OPTIONAL_RHS) {
		return do_operator(exp_state, final_result, op, this, NULL);
	}

	cerr << "While doing fastop '" << op->name << "'" << " on value of type " << get_type_name() << endl;
	throw runtime_error("Too few operands for operator");
}

int wpl_value::do_operator_recursive (
		wpl_expression_state *exp_state,
		wpl_value *final_result
		)
{
//	cout << "V (" << this << "): do_operator_recursive with " << exp_state->get_stack().size() << " on stack (type is " << get_type_name() << ")\n";

	if (exp_state->empty()) {
//		cout << "- no more elements\n";
		if (!exp_state->empty_waiting()) {
			while (!exp_state->empty_waiting()) {
				cerr << "- " << exp_state->top_waiting() << endl;
				exp_state->pop_waiting();
			}
			throw runtime_error("Operands remains in expression after all operators have completed");
		}
	/*	try {
			//cout << "Final result is: " << final_result->toString() << "\n";
		} catch (exception &e) {}*/

		if (final_result) {

//			cout << "- setting final result\n";
			try {
				return final_result->finalize_expression(exp_state, this);
			}
			catch (const wpl_value_no_weak_set &e) {}
		}
		return WPL_OP_OK;
	}

	shunting_yard_carrier first_carrier = exp_state->top();
	exp_state->pop();

	const wpl_operator_struct *op = first_carrier.op;
	wpl_value *value = first_carrier.value;

	if (value) {
		exp_state->push_waiting(this);
//		cout << "- next is value of type " << first_carrier.value->get_type_name() << "\n";
		return first_carrier.value->do_operator_recursive (exp_state, final_result);
	}

	if (op == &OP_BREAK_OP) {
		return (WPL_OP_OK|WPL_OP_BREAK);
	}
	else if (op == &OP_RETURN_OP) {
		if (!final_result) {
			throw runtime_error("wpl_value::do_operator_recursive(): return OP requested with NULL final_result");
		}

		try {
			if (!(final_result->set_strong (this))) {
				ostringstream msg;
				msg << "Strong set failed in return statement, possible type mismatch. " << 
					"Attemted to set return value of type '" <<
					final_result->get_type_name() << "' " <<
					"to value of type '" <<
					get_type_name() << "'\n";
				throw runtime_error(msg.str());
			}
		}
		catch (const wpl_value_no_strong_set &e) {
			final_result->set_weak(this);
		}
		return (WPL_OP_OK|WPL_OP_RETURN);
	}

	wpl_value *lhs = NULL;
	wpl_value *rhs = NULL;

//	cout << "- found op " << op->name << endl;

	if ((op->flags & WPL_OP_F_HAS_BOTH) == WPL_OP_F_HAS_BOTH) {
		wpl_value *prevprev = NULL;
		if (!exp_state->empty_waiting()) {
			prevprev = exp_state->top_waiting();
			exp_state->pop_waiting();
			//cout << "Got from waiting queue: " << prevprev << endl;
		}
		if (!prevprev && !(op->flags & (WPL_OP_F_OPTIONAL_LHS|WPL_OP_F_OPTIONAL_RHS))) {
			cerr << "While running operator '" << op->name << "' with prevprev '" << prevprev << "':\n";
			throw runtime_error("wpl_value::do_recursive_op(): Operator needs two arguments, one given");
		}
		lhs = prevprev;
		rhs = this;
	}
	else if (op->flags & WPL_OP_F_HAS_LHS) {
		lhs = this;
	}
	else if (op->flags & WPL_OP_F_HAS_RHS) {
		rhs = this;
	}

	wpl_value *preferred;

	if (op == &OP_FUNCTION_CALL) {
		if (lhs && (lhs->get_precedence() == wpl_type_precedence_function_ptr)) {
			preferred = lhs;
		}
		else {
			preferred = rhs;
		}
	}
	else if (op == &OP_ELEMENT) {
		preferred = lhs;
	}
	else if (!rhs) {
		preferred = lhs;
	}
	else if (!lhs) {
		preferred = rhs;
	}
	else if (WPL_OP_LEFT_PRECEDENCE (op)) {
		preferred = lhs;
	}
/*	else if (rhs->get_precedence() > lhs->get_precedence()) {
		preferred = rhs;
	}
	else if (lhs->get_precedence() > rhs->get_precedence()) {
		preferred = lhs;
	}*/
	else {
		preferred = lhs;
	}

/*	cout << "- preferred is " << preferred->get_type_name() << endl;
	cout << "- lhs type is " << (lhs?lhs->get_type_name():"-") << endl;
	cout << "- rhs type is " << (rhs?rhs->get_type_name():"-") << endl;
	cout << "- calling operator " << op->name << "\n";*/

	int ret_op;
	if (op == &OP_PATTERN_EQ || op == &OP_PATTERN_NOT_EQ) {
		ret_op = preferred->do_regex(exp_state, final_result, op, lhs, rhs);
	}
	else {
		ret_op = preferred->do_operator(exp_state, final_result, op, lhs, rhs);
	}
	if (ret_op & WPL_OP_UNKNOWN) {
		cerr << "While running operator '" << op->name << "' on type '" <<
			preferred->get_type_name() << "' in expression:\n";
		throw runtime_error("Unknown operator for type");
	}
	if (ret_op & WPL_OP_NAME_UNRESOLVED) {
		cerr << "While running operator '" << op->name << "' on type '" <<
			preferred->get_type_name() << "' in expression:\n";
		throw runtime_error("Unresolved identifier");
	}
	return ret_op;
}

int wpl_value::do_regex (
		wpl_expression_state *exp_state,
		wpl_value *final_result,
		const struct wpl_operator_struct *op,
		wpl_value *lhs,
		wpl_value *rhs
		)
{
	list<unique_ptr<wpl_value>> matches;

	string tmp = toString();
	bool match = rhs->do_pattern_match(exp_state, tmp, matches);

	for (const unique_ptr<wpl_value> &match : matches) {
		exp_state->push_discard(match.get());
	}

	if (op == &OP_PATTERN_NOT_EQ) {
		match = !match;
	}

	wpl_value_bool result(match);

	return result.do_operator_recursive(exp_state, final_result);
}

