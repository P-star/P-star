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

#include "hash.h"
#include "value_holder.h"

#include <string>

class wpl_type_complete;

extern const wpl_type_hash *wpl_type_global_hash;

class wpl_value_hash : public wpl_value_template, public wpl_hash {
	private:
	wpl_value *lhs;
	wpl_value *rhs;
	wpl_value *result;

	int discard() {result = lhs; return (WPL_OP_OK|WPL_OP_DISCARD|WPL_OP_RETURN_REFERENCE); }

	public:
	wpl_value_hash(const wpl_type_complete *hash_type, const wpl_type_complete *template_type) :
		wpl_value_template(hash_type, template_type)
	{}

	PRIMITIVE_TYPEATTR_TEMPLATE(hash)
	wpl_value_hash *clone() const {return new wpl_value_hash(*this); };
	wpl_value_hash *clone_empty() const {return new wpl_value_hash(container_type, template_type); }

	void notify_destructor (wpl_state *state, wpl_namespace_session *nss, wpl_io &io) override {
		wpl_hash::notify_destructor (state, nss, io);
	}
	/*
	int finalize_expression (wpl_expression_state *exp_state, wpl_value *last_value) override {
		if (!set_strong (last_value)) {
			cerr << "While setting final result of type " << get_type_name() <<
				" to hash of type " << last_value->get_type_name() << ":\n";
			throw runtime_error("Incompatible types");
		}

		return WPL_OP_OK;
	}*/

	bool set_strong (wpl_value *value);
	int do_operator (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const struct wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
	); 

	void output_json(wpl_io &io) override {
		wpl_hash::output_json(io);
	}

	bool isArray() {
		return true;
	}

	wpl_value *define(string &key);
};

