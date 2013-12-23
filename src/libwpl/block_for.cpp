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

#include "value_bool.h"
#include "block_for.h"
#include "expression.h"

#include <memory>

wpl_block_for::wpl_block_for() :
	exp_init(new wpl_expression()),
	exp_condition(new wpl_expression()),
	exp_increment(new wpl_expression_par_enclosed())
{}

int wpl_block_for::run(wpl_state *state, wpl_value *final_result) {
	int ret = WPL_OP_NO_RETURN;
	wpl_block_state *block_state = (wpl_block_state*) state;

	ret = block_state->run_init(exp_init.get(), final_result);

	wpl_value_bool result;
	while ((ret = block_state->run_run_condition(exp_condition.get(), &result) & WPL_OP_OK) &&
		result.get()
	) {
		ret = wpl_block::run(block_state, final_result);

		if (ret & (WPL_OP_BREAK|WPL_OP_RETURN)) {
			ret &= ~(WPL_OP_BREAK);
			break;
		}

		ret = block_state->run_increment(exp_increment.get(), final_result);
	}

	return ret;
}

void wpl_block_for::parse_value (wpl_namespace *ns) {
	set_parent_namespace(ns);

	ignore_whitespace();
	if (!ignore_letter ('(')) {
		THROW_ELEMENT_EXCEPTION("Expected '(' in for loop definition");
	}

	exp_init->load_position(get_position());
	exp_init->parse_value(ns);
	load_position(exp_init->get_position());

	exp_condition->load_position(get_position());
	exp_condition->parse_value(ns);
	load_position(exp_condition->get_position());

	// This expression catches the end parantheses ')'
	exp_increment->insert_fake_open_par();
	exp_increment->load_position(get_position());
	exp_increment->parse_value(ns);
	load_position(exp_increment->get_position());

	ignore_blockstart();
	wpl_block::parse_value(this);
}
