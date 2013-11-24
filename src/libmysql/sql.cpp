/*

-------------------------------------------------------------

Copyright (c) MMIII Atle Solbakken
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

#include "sql.h"
#include "../libwpl/exception.h"
#include "../libwpl/namespace.h"
#include "../libwpl/namespace_session.h"
#include "../libwpl/variable.h"
#include "value_sql.h"
#include "../libwpl/value_constant_pointer.h"

#include <string>
#include <iostream>
#include <utility>
#include <memory>
#include <list>

void wpl_sql::get_params(wpl_text_state *text_state, vector<wpl_value*> &params) {
	wpl_value_constant_pointer retriever;

	int index;
	for (chunk &my_chunk : chunks) {
		wpl_expression *exp;
		if (!(exp = my_chunk.get_expression())) {
			continue;
		}

		int ret = text_state->run_expression(exp, index, &retriever);
		wpl_value *value = retriever.dereference();

		if (!value) {
			throw runtime_error("SQL error: Only variables and constant values may be used in prepared statements");
		}

		params.push_back(value);
		index++;
	}
}

void wpl_sql::get_stmt_string(string &result) {
	/*
	   TODO might do some optimization here
	   */
	result.reserve(256);
	for (chunk &my_chunk : chunks) {
		if (my_chunk.get_start()) {
			result.append(my_chunk.get_start(), my_chunk.length());
		}
		else {
			result += '?';
		}
	}
}

void wpl_sql::parse_value (wpl_namespace *parent_namespace) {
	char buf[WPL_VARNAME_SIZE];
	get_word(buf);

	wpl_sql *sql = new wpl_sql();
	wpl_value_sql *value_sql = new wpl_value_sql(sql);

	wpl_variable_holder new_variable (value_sql, buf);

	parent_namespace->register_identifier(&new_variable);

	ignore_blockstart();

	sql->load_position(get_static_position());
	sql->__parse_value(parent_namespace);
	load_position(sql->get_static_position());
}

void wpl_sql::__parse_value (wpl_namespace *parent_namespace) {
	wpl_text::parse_value(parent_namespace);
}
