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

#include "block_state.h"
#include "block_while.h"
#include "expression.h"
#include "block.h"

wpl_state *wpl_block_while::new_state (wpl_namespace_session *nss, wpl_io *io) {
	 return new wpl_block_while_state(nss, io, this);
}

int wpl_block_while::run(wpl_state *state, wpl_value *final_result) {
	int ret = WPL_OP_NO_RETURN;

	wpl_block_while_state *block_state =
		(wpl_block_while_state*) state;

	while (check_run(block_state)) {
		ret = wpl_block_intermediate::run(state, final_result);
		block_state->reset();

		if (ret & (WPL_OP_BREAK|WPL_OP_RETURN)) {
			ret &= ~(WPL_OP_BREAK);
			break;
		}
	}

	return ret;
}

void wpl_block_while::parse_value(wpl_namespace *ns) {
	set_parent_namespace (ns);

	wpl_block *block = new wpl_block();
	set_runable(block);

	wpl_expression_par_enclosed *exp = new wpl_expression_par_enclosed();
	run_condition.reset(exp);

	ignore_whitespace();
	if (!ignore_letter ('(')) {
		THROW_ELEMENT_EXCEPTION("Expected '(' in foreach while definition");
	}

	exp->insert_fake_open_par();
	exp->load_position(get_position());
	exp->parse_value(this);
	load_position(exp->get_position());

	ignore_blockstart();

	block->set_parent_namespace(this);
	block->load_position(get_position());
	block->parse_value(block);
	load_position(block->get_position());
}
