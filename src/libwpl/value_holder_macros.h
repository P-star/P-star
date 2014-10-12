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

#pragma once

#define RESULT_LOGIC wpl_value_holder<A>::result_logic
#define RESULT wpl_value_holder<A>::result
#define VALUE wpl_value_holder<A>::value
#define LHS wpl_value_holder<A>::lhs_tmp
#define RHS wpl_value_holder<A>::rhs_tmp

#define CALL_OP(_op, func)									\
	if (op == &_op) { return func(); }

#define PRIMITIVE_CONSTRUCTOR(type,shortname,parent)						\
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

#define PRIMITIVE_SET_WEAK_REAL(type,shortname,translator,notify)				\
	void set_weak (wpl_value *new_value) {							\
		value = new_value->translator;							\
		notify										\
	}

#define PRIMITIVE_SET_WEAK(type,shortname,translator)						\
	PRIMITIVE_SET_WEAK_REAL(type,shortname,translator,)

#define PRIMITIVE_SET_WEAK_NOTIFY(type,shortname,translator)					\
	PRIMITIVE_SET_WEAK_REAL(type,shortname,translator,notify_parasites();)

#define PRIMITIVE_DO_OPERATOR_REAL(shortname,translator,notify)					\
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
		if (op == &OP_POINTERTO) {							\
			wpl_value_pointer result(exp_state->get_nss(), get_type(), this);	\
			return result.do_operator_recursive(exp_state, final_result);		\
		}										\
		if (lhs) {									\
			set_lhs_value(lhs->translator);			 			\
		}										\
		if (rhs) {									\
			set_rhs_value(rhs->translator);			 			\
		}										\
		int ret = __do_operator (							\
			op									\
		);										\
		if (ret & WPL_OP_DATA_MODIFIED) {						\
			notify									\
		}										\
		/*cerr << "Logic result is: " << result_logic << endl;*/			\
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
			if (ret & WPL_OP_ASSIGN) {						\
				lhs->set_weak(&mytype_result);					\
			}									\
			else if (ret & WPL_OP_DISCARD) {					\
				return do_operator_discard(exp_state, &mytype_result, final_result); \
			}									\
			return mytype_result.do_operator_recursive(				\
					exp_state, final_result					\
			);									\
		}										\
		if (ret & WPL_OP_UNKNOWN) {							\
			cerr << "In operator '" << op->name <<					\
				"' in type '" << wpl_typename_##shortname << "':\n";		\
			throw runtime_error("Unknown operator for this type");			\
		}										\
/*		cout << "V(" << this << "): no return from operator\n";				*/\
		return ret;									\
	}	

#define PRIMITIVE_DO_OPERATOR(shortname,translator)						\
	PRIMITIVE_DO_OPERATOR_REAL(shortname,translator,)

#define PRIMITIVE_DO_OPERATOR_NOTIFY(shortname,translator)					\
	PRIMITIVE_DO_OPERATOR_REAL(shortname,translator,notify_parasites();)

#define PRIMITIVE_TYPEATTR_TEMPLATE(shortname)							\
	int get_precedence() const override { return wpl_type_precedence_##shortname; }		\
	const char *get_type_name() const override { return container_type->get_name(); }	\
	const wpl_type_complete *get_type() const override { return container_type; }

#define PRIMITIVE_TYPEATTR(shortname)								\
	int get_precedence() const override { return wpl_type_precedence_##shortname; }		\
	const char *get_type_name() const override { return wpl_typename_##shortname; }

#define PRIMITIVE_GETTYPE(shortname)								\
	const wpl_type_complete *get_type() const override {					\
		return (const wpl_type_complete*) wpl_type_global_##shortname;			\
	}

#define PRIMITIVE_TYPEINFO(shortname)								\
	PRIMITIVE_TYPEATTR(shortname)								\
	PRIMITIVE_GETTYPE(shortname)

#define PRIMITIVE_CLASS_CONTENT(type,shortname,translator,parent)				\
	public:											\
	PRIMITIVE_CONSTRUCTOR(type,shortname,parent)						\
	PRIMITIVE_TYPEINFO(shortname)								\
	PRIMITIVE_SET_WEAK(type,shortname,translator)						\
	PRIMITIVE_DO_OPERATOR(shortname,translator)
