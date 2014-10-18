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

#include "user_function.h"
#include "type_parse_signals.h"
#include "block.h"

wpl_user_function::wpl_user_function(
		const wpl_type_complete *return_type,
		const char *name,
		int access_flags
) :
	wpl_function(return_type, name, access_flags)
{}

int wpl_user_function::run (wpl_state *state, wpl_value *final_result) {
	wpl_function_state *function_state = (wpl_function_state*) state;
	return function_state->run_block(&block, final_result);
}

void wpl_user_function::parse_value(wpl_namespace *ns) {
	set_parent_namespace(ns);
	wpl_matcher_position begin_pos(get_position());

	ignore_whitespace();
	if (ignore_letter (')')) {
		goto no_arguments;
	}

	do {
		char buf[WPL_VARNAME_SIZE];
		try {
			get_word(buf);
		}
		catch (wpl_element_exception &e) {
			cerr << "While looking for word in function argument list:\n";
			throw;
		}

		wpl_parseable *parseable;
		if (!(parseable = find_parseable (buf))) {
			load_position(begin_pos);
			cerr << "While parsing name '" << buf <<
				"' inside function argument declaration of function '" << get_name() <<
				"':\n";
			THROW_ELEMENT_EXCEPTION("Undefined name");
		}

		parseable->load_position(get_position());

		try {
			parseable->parse_value(this);
		}
		catch (wpl_type_begin_variable_declaration &e) {
			e.create_variable(this);
			load_position(e.get_position());
		}

		ignore_whitespace();
		if (ignore_letter (')')) {
			break;
		}
		else if (ignore_letter (',')) {
			continue;
		}
		else {
			load_position(begin_pos);
			THROW_ELEMENT_EXCEPTION("Syntax error in function argument declaration");
		}
	} while (!at_end());

	no_arguments:

	ignore_whitespace();

	block.load_position(get_position());
	block.parse_value(this);
	load_position(block.get_position());

	generate_signature();
}
