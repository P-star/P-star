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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with P*.  If not, see <http://www.gnu.org/licenses/>.

-------------------------------------------------------------

*/

#include "block_state.h"
#include "block_foreach.h"
#include "block.h"
#include "value_constant_pointer.h"

wpl_state *wpl_block_foreach::new_state (wpl_state *parent, wpl_namespace_session *nss, wpl_io *io) {
	 return new wpl_block_foreach_state(parent, nss, io, this, get_block());
}

int wpl_block_foreach::run(wpl_state *state, wpl_value *final_result) {
	int ret = WPL_OP_NO_RETURN;
	wpl_block_foreach_state *block_state =
		(wpl_block_foreach_state*) state;

	wpl_value *var;
	wpl_value_constant_pointer constant_pointer;
	constant_pointer.set_do_finalize();
	if (!(ret = block_state->run_init(exp_init.get(), &constant_pointer)) & WPL_OP_OK) {
		return ret;
	}
	if ((var = constant_pointer.dereference()) == NULL) {
		throw runtime_error("No constant found in foreach loop variable section");
	}
	var->set_do_finalize();

	int i = 0;
	int condition_ret;
	while ((condition_ret = block_state->run_condition(run_condition.get(), var, i)) & WPL_OP_OK) {
		if (condition_ret & (WPL_OP_BREAK|WPL_OP_RANGE_ABORT)) {
			return condition_ret & ~(WPL_OP_BREAK|WPL_OP_RANGE_ABORT) | ret;
		}

		ret = wpl_block_intermediate::run(block_state, final_result);
		if (ret & (WPL_OP_BREAK|WPL_OP_RETURN)) {
			return ret & ~WPL_OP_BREAK;
		}
		if (!(condition_ret & WPL_OP_RANGE)) {
			return ret;
		}
		if (condition_ret & WPL_OP_RANGE_COMPLETE) {
			return ret;
		}
		i++;
	}

	return ret;
}

void wpl_block_foreach::parse_value(wpl_namespace *ns) {
	set_parent_namespace (ns);

	exp_init.reset(new wpl_expression());

	wpl_expression_par_enclosed *exp = new wpl_expression_par_enclosed();
	run_condition.reset(exp);

	ignore_whitespace();
	if (!ignore_letter ('(')) {
		THROW_ELEMENT_EXCEPTION("Expected '(' in foreach loop definition");
	}

	/*
	   Allow declaration of variables inside init statement
	   */
	find_and_parse_complete_type();

	exp_init->load_position(get_position());
	exp_init->parse_value(this);
	load_position(exp_init->get_position());

	exp->insert_fake_open_par();
	exp->load_position(get_position());
	exp->parse_value(this);
	load_position(exp->get_position());

	get_block()->load_position(get_position());
	get_block()->parse_value(this);
	load_position(get_block()->get_position());
}
