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

#pragma once

#include "type.h"
#include "pointer.h"
#include "operator_types.h"
#include "value_holder_macros.h"

class wpl_namespace_session;
extern const wpl_type_pointer *wpl_type_global_pointer;

class wpl_value_pointer : public wpl_pointer, public wpl_value_template {
	public:
	PRIMITIVE_TYPEATTR_TEMPLATE(pointer)
	wpl_value_pointer *clone() const {return new wpl_value_pointer(*this); };
	wpl_value_pointer *clone_empty() const {return new wpl_value_pointer(get_container(), get_template()); }

	wpl_value_pointer (
			wpl_namespace_session *nss,
			const wpl_type_complete *template_type,
			wpl_value *value
	) :
		wpl_pointer(value),
		wpl_value_template (
			nss,
			shared_ptr<const wpl_type_complete>(wpl_type_global_pointer->new_instance(template_type)),
			template_type
		)
	{}

	wpl_value_pointer(
			const wpl_type_complete *pointer_type,
			const wpl_type_complete *template_type
	) :
		wpl_pointer(),
		wpl_value_template(pointer_type, template_type)
	{}
	wpl_value_pointer(
			const wpl_type_complete *pointer_type,
			const wpl_type_complete *template_type,
			wpl_value *value
	) :
		wpl_pointer(value),
		wpl_value_template(pointer_type, template_type)
	{}

	int do_operator (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const struct wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
	);

	int finalize_expression (wpl_expression_state *exp_state, wpl_value *last_value) override;
	void set_weak(wpl_value *value) override;
	virtual void output_json(wpl_io &io) override;
	bool isPointer() override {
		return true;
	}
};

