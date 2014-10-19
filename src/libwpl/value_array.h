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

#include "array.h"
#include "value_holder.h"

extern const wpl_type_array *wpl_type_global_array;

class wpl_value_array : public wpl_value_template, public wpl_array {
	private:
	wpl_value *lhs;
	wpl_value *rhs;
	wpl_value *result;

	wpl_value *define_if_needed(int index);
	int discard() {result = lhs; return (WPL_OP_OK|WPL_OP_DISCARD|WPL_OP_RETURN_REFERENCE); }

	public:
	wpl_value_array(const wpl_value_array &copy) :
		wpl_value_template(copy),
		wpl_array(copy)
	{}

	wpl_value_array(const wpl_type_complete *array_type, const wpl_type_complete *template_type, int length) :
		wpl_value_template(array_type, template_type)
	{
		reserve(length);
	}

	wpl_value_array(const wpl_type_complete *array_type, const wpl_type_complete *template_type) :
		wpl_value_template(array_type, template_type)
	{}

	PRIMITIVE_TYPEATTR_TEMPLATE(array)
	wpl_value_array *clone() const {return new wpl_value_array(*this); };
	wpl_value_array *clone_empty() const {return new wpl_value_array(container_type, template_type); }

	void push_weak(wpl_value *value);

	void notify_destructor(wpl_state *state, wpl_namespace_session *nss, wpl_io &io) override {
		wpl_array::notify_destructor(state, nss, io);
	}

	void reset() override {
		clear();
	};

	bool set_strong (wpl_value *value);
	int do_operator (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const struct wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
	); 

	int do_operator_recursive (
			wpl_expression_state *exp_state,
			wpl_value *final_result
	) override;


	int finalize_expression (wpl_expression_state *exp_state, wpl_value *last_value) override;

	void output_json(wpl_io &io) override {
		wpl_array::output_json(io);
	}

	bool isArray() {
		return true;
	}


#ifdef WPL_DEBUG_EXPRESSIONS
	string toString() const {
		return string("DBG{array ") + template_type->get_name() + "}";
	}
#endif
};

