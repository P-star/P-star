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

#include "value.h"
#include "value_bool.h"
#include "value_string.h"
#include "pointer.h"
#include "type_precedence.h"
#include "expression_state.h"
#include "operator.h"

/*
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
*/
void wpl_value::invalidate_pointers() {
	for (auto ptr : pointers) {
		ptr->value_dies_now();
	}
}

void wpl_value::register_pointer(wpl_pointer *ptr) {
	pointers.push_back(ptr);
}

void wpl_value::remove_pointer(wpl_pointer *ptr) {
	for (auto it = pointers.begin(); it != pointers.end(); ++it) {
		if (*it == ptr) {
			pointers.erase(it);
			break;
		}
	}
}

void wpl_value::suicide() {
	delete this;
}

wpl_value::~wpl_value() {
	invalidate_pointers();
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
	int ret;

	if ((op->flags & WPL_OP_F_HAS_BOTH) != WPL_OP_F_HAS_BOTH) {
		ret = do_operator(exp_state, final_result, op, this, this);
	}
	else if (op->flags & WPL_OP_F_OPTIONAL_LHS) {
		ret = do_operator(exp_state, final_result, op, NULL, this);
	}
	else if (op->flags & WPL_OP_F_OPTIONAL_RHS) {
		ret = do_operator(exp_state, final_result, op, this, NULL);
	}
	else {
		cerr << "While doing fastop '" << op->name << "'" << " on value of type " << get_type_name() << " value " << toString() << endl;
		throw runtime_error("Too few operands for operator");
	}

	if (ret & WPL_OP_UNKNOWN) {
		cerr << "While running fastop '" << op->name << "' on type '" <<
			get_type_name() << "' in expression:\n";
		throw runtime_error("Unknown operator for type");
	}

	return ret;
}

int wpl_value::do_operator_discard (
		wpl_expression_state *exp_state,
		wpl_value *discarded,
		wpl_value *final_result
){
	exp_state->push_discard(discarded);
	if (exp_state->empty()) {
		return WPL_OP_OK;
	}

	shunting_yard_carrier next = exp_state->top();
	if (!next.value){
		throw runtime_error("Operator can't follow discard operator");
	}
	exp_state->pop();

	return next.value->do_operator_recursive (
		exp_state, final_result
	);
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
//				cerr << "- " << exp_state->top_waiting() << endl;
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

	shunting_yard_carrier &first_carrier = exp_state->top();
	exp_state->pop();

	const wpl_operator_struct *op = first_carrier.op;
	wpl_value *value = first_carrier.value;

	if (value) {
		exp_state->push_waiting(this);
		//cout << "- next is value of type " << first_carrier.value->get_type_name() << "\n";
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
//			cout << "Got from waiting queue: " << prevprev->get_type_name() << endl;
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
		if (get_precedence() == wpl_type_precedence_function_ptr) {
			if (lhs == this) {
				preferred = lhs;
			}
			else {
				preferred = rhs;
			}
		}
		else if (lhs && (lhs->get_precedence() == wpl_type_precedence_function_ptr)) {
			preferred = lhs;
		}
		else {
			preferred = rhs;
		}
	}
	else if (op->flags & WPL_OP_F_ASSOC_LEFT) {
		preferred = lhs;
	}
	else if (op->flags & WPL_OP_F_ASSOC_RIGHT) {
		preferred = rhs;
	}
	else {
		ostringstream msg;
		msg << "Don't know which value to run operator " << op->name << " on!!\n";
		throw runtime_error(msg.str());
	}

/*	cout << "- preferred is " << preferred << " - " << preferred->get_type_name() << endl;
	cout << "- lhs type is  " << lhs       << " - " << (lhs?lhs->get_type_name():"-") << endl;
	cout << "- rhs type is  " << rhs       << " - " << (rhs?rhs->get_type_name():"-") << endl;
	cout << "- calling operator " << op->name << "\n";*/

	int ret_op;
	if (op->flags & WPL_OP_F_HAS_RUNABLE) {
		if (first_carrier.runable.get() == nullptr) {
			wpl_runable_operator *runable = op->new_runable(exp_state, lhs, rhs);
			first_carrier.runable.reset(runable);
		}
		ret_op = exp_state->run_runable_operator(
				first_carrier.runable.get(),
				exp_state->pos()+1,
				lhs,
				rhs,
				final_result
		);
	}
	else if (op == &OP_PATTERN_EQ || op == &OP_PATTERN_NOT_EQ) {
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
	wpl_value_string new_value(tmp);

	set_weak(&new_value);

	for (const unique_ptr<wpl_value> &match : matches) {
		exp_state->push_discard(match.get());
	}

	if (op == &OP_PATTERN_NOT_EQ) {
		match = !match;
	}

	wpl_value_bool result(match);

	return result.do_operator_recursive(exp_state, final_result);
}

wpl_value_template::wpl_value_template (
		wpl_namespace_session *nss,
		shared_ptr<const wpl_type_complete> mother_type,
		const wpl_type_complete *template_type
) : wpl_value() {
	const wpl_type_complete *tmp;

	/*
	   If this type doesn't exist in the namespace, we must hold memory
	   for it inside this class
	   */
	if (!(tmp = nss->find_complete_type(mother_type->get_name()))) {
		temporary_type = mother_type;
		tmp = mother_type.get();
	}

	this->container_type = tmp;
	this->template_type = template_type;
}

