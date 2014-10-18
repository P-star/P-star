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

#include "state.h"
#include "value.h"
#include "type.h"
#include "namespace_session.h"
#include "block_state.h"
#include "value_struct.h"

#include <memory>

class wpl_block;
class wpl_expression_state;
class wpl_value;

class wpl_function_state : public wpl_state, public wpl_namespace_session {
	private:
	unique_ptr<wpl_value> return_value;
	unique_ptr<wpl_block_state> block_state;

	public:
	wpl_function_state (
		wpl_state *parent,
		wpl_namespace_session *nss_this,
		wpl_io *io,
		wpl_namespace *template_namespace,
		const wpl_type_complete *return_type
	) :
		wpl_state(parent, nss_this, io),
		wpl_namespace_session(nss_this, nss_this, template_namespace, WPL_NSS_CTX_SELF),
		return_value(return_type->new_instance())
	{}
	wpl_function_state (
		wpl_state *parent,
		wpl_namespace_session *nss_call_ctx,
		wpl_namespace_session *nss_this,
		wpl_io *io,
		wpl_namespace *template_namespace,
		const wpl_type_complete *return_type
	) :
		wpl_state(parent, nss_this, io),
		wpl_namespace_session(nss_call_ctx, nss_this, template_namespace, WPL_NSS_CTX_SELF),
		return_value(return_type->new_instance())
	{}
	int run_block(wpl_block *block, wpl_value *final_result);
	wpl_value *get_return_value() {
		return return_value.get();
	}

	wpl_value *find_variable_value (const char *function_name, const char *var_name) {
		wpl_variable *var;
		if (!(var = find_variable(var_name, WPL_NSS_CTX_SELF))) {
			throw runtime_error(string(function_name) + " error: Could not find '" + var_name + "' variable");
		}

		return var->get_value();
	}

	template<class T> T *find_variable_value (const char *function_name, const char *var_name) {
		wpl_variable *var;
		if (!(var = find_variable(var_name, WPL_NSS_CTX_SELF))) {
			throw runtime_error(string(function_name) + " error: Could not find '" + var_name + "' variable");
		}

		T *out_var = dynamic_cast<T*>(var->get_value());
		if (!out_var) {
			T tmp;
			throw runtime_error(string(function_name) + " error: " +
					" Argument was of type '" + var->get_value()->get_type_name() + "'" +
					", but '" + tmp.get_type_name() + "' was expected"
			);
		}
		return out_var;
	}
};
