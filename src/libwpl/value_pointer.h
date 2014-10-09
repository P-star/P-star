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

#pragma once

#include "pointer.h"
#include "operator_types.h"
#include "value_holder_macros.h"

extern const wpl_type_pointer *wpl_type_global_pointer;

class wpl_value_pointer : public wpl_value, public wpl_pointer {
	public:
	PRIMITIVE_TYPEINFO(pointer)
	wpl_value_pointer *clone() const {return new wpl_value_pointer(*this); };
	wpl_value_pointer *clone_empty() const {return new wpl_value_pointer(get_template_type()); }

	wpl_value_pointer(const wpl_type_complete *type) :
		wpl_pointer(type), wpl_value()
	{}
	wpl_value_pointer(const wpl_type_complete *type, wpl_value *value) :
		wpl_pointer(type, value), wpl_value()
	{}

	int do_operator (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const struct wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
	);

	int finalize_expression (wpl_expression_state *exp_state, wpl_value *last_value) override {
		set_value(last_value);
		return WPL_OP_OK;
	}

	void set_weak(wpl_value *value) override;
};

