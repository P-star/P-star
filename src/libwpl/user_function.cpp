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

#include "user_function.h"
#include "block.h"

wpl_user_function::wpl_user_function(const char *name, const wpl_type_complete *return_type)
	: wpl_function(name, return_type)
{
	block = new wpl_block();
}

wpl_user_function::~wpl_user_function() {
	delete block;
}

int wpl_user_function::run (wpl_state *state, wpl_value *final_result) {
	wpl_function_state *function_state = (wpl_function_state*) state;
	return function_state->run_block(block, final_result);
}

void wpl_user_function::parse_value(wpl_namespace *ns) {
	set_parent_namespace(ns);
	block->load_position(get_static_position());
	block->parse_function_arguments(this);
	load_position(block->get_static_position());

	ignore_string_match (WHITESPACE, 0);

	if (!ignore_letter ('{')) {
		THROW_ELEMENT_EXCEPTION("Expected definition of function block after declaration");
	}

	block->load_position(get_static_position());
	block->parse_value(block);
	load_position(block->get_static_position());

	generate_signature();
}
