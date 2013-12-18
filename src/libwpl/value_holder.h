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

#pragma once

#include "expression_state.h"
#include "operator.h"
#include "value.h"

#define RESULT_LOGIC wpl_value_holder<A>::result_logic
#define RESULT wpl_value_holder<A>::result
#define LHS *wpl_value_holder<A>::lhs_value
#define RHS *wpl_value_holder<A>::rhs_value

#define CALL_OP(_op, func)									\
	if (op == &_op) { return func(); }

#define PRIMITIVE_CONSTRUCTOR(type,shortname)							\
	wpl_value_##shortname () {								\
		zero();										\
	}											\
	wpl_value_##shortname (const type &new_value) {						\
		value = new_value;								\
       	}											\
	wpl_value_##shortname (const wpl_value_##shortname &copy) {				\
		value = copy.value;								\
       	}											\
	wpl_value_##shortname *clone() const override {						\
		return new wpl_value_##shortname(*this);					\
	}											\
	wpl_value_##shortname *clone_empty() const override {					\
		return new wpl_value_##shortname();						\
	}											\

#define PRIMITIVE_SET_WEAK(type,shortname,translator)						\
	void set_weak (wpl_value *new_value) {							\
		value = new_value->translator;							\
	}
#define PRIMITIVE_DO_OPERATOR(shortname,translator)						\
	int do_operator (									\
			wpl_expression_state *exp_state,					\
			wpl_value *final_result,						\
			const struct wpl_operator_struct *op,					\
			wpl_value *lhs,								\
			wpl_value *rhs								\
	) {											\
		/*cout << "V(" << this << "): do_operator op " << op->name << "\n";		*/\
		/*cout << " - lhs is " << (lhs) << endl;					*/\
		/*cout << " - rhs is " << (rhs) << endl;					*/\
		/*cout << " - exp state wait top is empty: " << exp_state.empty_waiting() << endl;*/\
		if (lhs) {									\
			set_lhs_value(lhs, lhs->translator);			 		\
		}										\
		if (rhs) {									\
			set_rhs_value(rhs, rhs->translator);			 		\
		}										\
		int ret = __do_operator (							\
			op									\
		);										\
		if ((ret & WPL_OP_LOGIC_OK) == WPL_OP_LOGIC_OK) {				\
			/*cout << "V(" << this << "): result is boolean logic " << result_logic << endl;*/\
			wpl_value_bool bool_result(result_logic);				\
			return bool_result.do_operator_recursive(				\
					exp_state, final_result					\
					);							\
		}										\
		if (ret & WPL_OP_OK) {								\
/*			cout << "V(" << this << "): result is " << result << endl;		*/\
			wpl_value_##shortname mytype_result(result);				\
			if (ret & WPL_OP_DISCARD) {						\
				exp_state->push_discard(&mytype_result);				\
				if (exp_state->empty()) {					\
					return ret;						\
				}								\
				shunting_yard_carrier next = exp_state->top();			\
				if (!next.value){						\
					throw runtime_error("Operator can't follow discard operator");\
				}								\
				exp_state->pop();						\
				return next.value->do_operator_recursive(				\
						exp_state, final_result				\
						);						\
			}									\
			else {									\
				return mytype_result.do_operator_recursive(			\
						exp_state, final_result				\
						);						\
			}									\
		}										\
		if (ret & WPL_OP_UNKNOWN) {							\
			cerr << "In operator '" << op->name <<					\
				"' in type '" << wpl_typename_##shortname << "':\n";		\
			throw runtime_error("Unknown operator for this type");			\
		}										\
/*		cout << "V(" << this << "): no return from operator\n";				*/\
		return ret;									\
	}	

#define PRIMITIVE_TYPEINFO(shortname)								\
	int get_precedence() const { return wpl_type_precedence_##shortname; }			\
	const char *get_type_name() const { return wpl_typename_##shortname; }

#define PRIMITIVE_CLASS_CONTENT(type,shortname,translator)					\
	public:											\
	PRIMITIVE_CONSTRUCTOR(type,shortname)							\
	PRIMITIVE_TYPEINFO(shortname)								\
	PRIMITIVE_SET_WEAK(type,shortname,translator)						\
	PRIMITIVE_DO_OPERATOR(shortname,translator)						\

template<typename A> class wpl_value_holder : public wpl_value {
	private:
	A lhs_tmp;
	A rhs_tmp;

	protected:
	int discard_op() {
		result = *lhs_value;
		return (WPL_OP_OK|WPL_OP_DISCARD);
	}
	int return_op() {
		result = *rhs_value;
		return (WPL_OP_OK|WPL_OP_RETURN);
	}
	int block_return_op() {
		result = *rhs_value;
		return (WPL_OP_OK|WPL_OP_BLOCK_RETURN);
	}

	A value;

	bool result_logic;
	A result;
	A *lhs_value;
	A *rhs_value;

	void set_lhs_value (wpl_value *lhs, A value_lhs
	) {
		lhs_tmp = value_lhs;
		lhs_value = (lhs == (wpl_value*) this ? &value : &lhs_tmp);
	}

	void set_rhs_value (wpl_value *rhs, A value_rhs
	) {
		rhs_tmp = value_rhs;
		rhs_value = (rhs == (wpl_value*) this ? &value : &rhs_tmp);
	}

	void set_values_to_myself() {
		lhs_value = &result;
		rhs_value = &result;
	}

	virtual int __do_operator (const struct wpl_operator_struct *op) {
		CALL_OP(OP_DISCARD, discard_op)
		CALL_OP(OP_RETURN_OP, return_op)
		CALL_OP(OP_BLOCK_RETURN_OP, block_return_op)
		return WPL_OP_UNKNOWN;
	}

	public:
	virtual ~wpl_value_holder() {}
	A get () { return value; }
	void set (A value) {
		this->value = value;
	}
};
