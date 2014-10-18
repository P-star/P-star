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

#include "block_for.h"
#include "block_state.h"
#include "block.h"

wpl_state *wpl_block_for::new_state (wpl_state *parent, wpl_namespace_session *nss, wpl_io *io) {
	 return new wpl_block_for_state(parent, nss, io, this, get_block());
}

int wpl_block_for::run(wpl_state *state, wpl_value *final_result) {
	int ret = WPL_OP_NO_RETURN;
	wpl_block_for_state *block_state =
		(wpl_block_for_state*) state;

	ret = block_state->run_init(exp_init.get(), final_result);

	while (check_run(block_state)) {
		ret = wpl_block_intermediate::run(block_state, final_result);

		if (ret & (WPL_OP_BREAK|WPL_OP_RETURN)) {
			ret &= ~(WPL_OP_BREAK);
			break;
		}

		ret = block_state->run_continue(exp_continue.get(), final_result);
	}

	return ret;
}

void wpl_block_for::parse_value (wpl_namespace *ns) {
	set_parent_namespace(ns);

	/*
	   Three expressions, like for (init ; condition ; continue) {
	   */
	exp_init.reset(new wpl_expression());
	exp_continue.reset(new wpl_expression_par_enclosed());
	wpl_expression *exp = new wpl_expression();
	run_condition.reset(exp);

	ignore_whitespace();
	if (!ignore_letter ('(')) {
		THROW_ELEMENT_EXCEPTION("Expected '(' in for loop definition");
	}

	/*
	   Allows us to declare a variable inside the init section
	   */
	find_and_parse_complete_type();

	exp_init->load_position(get_position());
	exp_init->parse_value(ns);
	load_position(exp_init->get_position());

	exp->load_position(get_position());
	exp->parse_value(ns);
	load_position(exp->get_position());

	// This expression catches the end parantheses ')'
	exp_continue->insert_fake_open_par();
	exp_continue->load_position(get_position());
	exp_continue->parse_value(ns);
	load_position(exp_continue->get_position());

	get_block()->load_position(get_position());
	get_block()->parse_value(this);
	load_position(get_block()->get_position());
}
