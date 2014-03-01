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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with P*. If not, see <http://www.gnu.org/licenses/>.

-------------------------------------------------------------

*/

#include "type_file.h"

#include "types.h"
#include "exception.h"
#include "function.h"

void file_open (wpl_namespace_session *nss) {
	throw runtime_error("FILE->open() not implemented");
}

class wpl_file_open : public wpl_function {
	public:
	wpl_file_open() :
		wpl_function(wpl_type_global_bool, "open", WPL_VARIABLE_ACCESS_PUBLIC)
	{
//		wpl_variable_holder sql("sql", new wpl_value_sql(), WPL_VARIABLE_ACCESS_PRIVATE);
//		register_identifier(&sql);
	}
	int run (wpl_state *state, wpl_value *final_result);
};

int wpl_file_open::run (
	wpl_state *state,
	wpl_value *final_result
) {
	wpl_variable *this_var;

	wpl_function_state *function_state = (wpl_function_state*) state;

	if (!(this_var = function_state->find_variable("this", WPL_NSS_CTX_SELF))) {
		throw runtime_error("FILE error: open(): Could not find 'this' variable");
	}

	file_open (function_state->get_parent());

	return WPL_OP_OK;
}

wpl_type_file::wpl_type_file() :
	wpl_struct(wpl_typename_file, true)
{
//	wpl_variable_holder this_var("this", new wpl_value_MYSQL_STMT(), WPL_VARIABLE_ACCESS_PRIVATE);
//	register_identifier(&this_var);

	register_identifier(new wpl_file_open());
}

wpl_type_file constant_type_file;

void wpl_type_file_register (wpl_namespace *target) {
	target->new_register_parseable(&constant_type_file);
}
