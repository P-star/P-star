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

#include "array.h"
#include "value_holder.h"

class wpl_value_array : public wpl_value_template, public wpl_array {
	private:
	wpl_value *lhs;
	wpl_value *rhs;
	wpl_value *result;

	wpl_value *define_if_needed(int index);
	int array_subscripting();
	int discard() {result = lhs; return (WPL_OP_OK|WPL_OP_DISCARD|WPL_OP_RETURN_REFERENCE); }

	public:
	PRIMITIVE_TYPEINFO(array)
	wpl_value_array *clone() const {return new wpl_value_array(*this); };
	wpl_value_array *clone_empty() const {return new wpl_value_array(template_type); }

	int finalize_expression (wpl_expression_state *exp_state, wpl_value *last_value) override {
		/*
		   TODO
		   Set array by discard chain?
		 */

		if (!set_strong (last_value)) {
			cerr << "While setting final result of type " << get_type_name() <<
				" to array of type " << last_value->get_type_name() << ":\n";
			throw runtime_error("Incompatible types");
		}

		return WPL_OP_OK;
	}

	bool set_strong (wpl_value *value);
	int do_operator (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const struct wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
	); 

	void output_json(wpl_io &io) override {
		wpl_array::output_json(io);
	}

	wpl_value_array(const wpl_value_array &copy) :
		wpl_value_template(copy),
		wpl_array(copy)
	{}

	wpl_value_array(const wpl_type_complete *template_type, int length) :
		wpl_value_template(template_type)
	{
		reserve(length);
	}

	wpl_value_array(const wpl_type_complete *template_type) :
		wpl_value_template(template_type)
	{}

	bool isArray() {
		return true;
	}


#ifdef WPL_DEBUG_EXPRESSIONS
	string toString() {
		return string("DBG{array ") + template_type->get_name() + "}";
	}
#endif
};

