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

#include "operator.h"
#include "value_holder.h"
#include "struct.h"
#include "exception.h"
#include "identifier.h"
#include "variable.h"
#include "namespace_session.h"

#include <iostream>

class wpl_value_struct : public wpl_value, public wpl_namespace_session {
	protected:

	const wpl_struct *mother_struct;
	wpl_variable *this_ptr;
	bool first_run;

	public:
	wpl_value_struct (const wpl_struct *mother_struct) :
		wpl_namespace_session(mother_struct),
		wpl_value()
	{
		this_ptr = NULL;
		this->mother_struct = mother_struct;
		first_run = true;
	}
	virtual ~wpl_value_struct() {}
	virtual void suicide() {
		delete this;
	}
	virtual const char *get_type_name() const { return mother_struct->get_name(); };
	virtual int get_precedence() const { return mother_struct->get_precedence(); };
	virtual wpl_value_struct *clone() const { return new wpl_value_struct(*this); };
	virtual wpl_value_struct *clone_empty() const { return new wpl_value_struct(*this); };

	const wpl_type_complete *get_type() const override { return mother_struct; };

	void reset() override {
		wpl_namespace_session::reset_variables();
		first_run = true;
	}

	void set_ctor_called() {
		first_run = false;
	}

	int finalize_expression (wpl_expression_state *exp_state, wpl_value *last_value) {
		if (!set_strong (last_value)){
			cerr << "While setting final result of type " << get_type_name() <<
				" to struct of type " << last_value->get_type_name() << ":\n";
			throw runtime_error("Incompatible types");
		}
		return WPL_OP_OK;
	}

	wpl_variable *get_this() {
		if (!this_ptr) {
			this_ptr = find_variable("this", WPL_NSS_CTX_SELF);
		}
		return this_ptr;
	}

	bool set_strong (wpl_value *value);

	int do_operator_recursive (
			wpl_expression_state *exp_state,
			wpl_value *final_result
	) override;
	int do_operator (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const struct wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
	) override;

	void output_json(wpl_io &io) override;

	const wpl_type *get_mother_struct() {
		return mother_struct;
	}

	bool isStruct() {
		return true;
	}

	void notify_destructor(wpl_state *state, wpl_namespace_session *nss, wpl_io &io) override;

#ifdef WPL_DEBUG_EXPRESSIONS
	string toString() const {
		return string("DBG{struct ") + mother_struct->get_name() + "}";
	}
#endif
};
