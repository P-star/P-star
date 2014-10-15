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

#include "expression.h"
#include "block_state.h"
#include "block_if.h"
#include "block.h"

#include "blocks.h"

wpl_state *wpl_block_if::new_state (wpl_namespace_session *nss, wpl_io *io) {
	 return new wpl_block_if_state(nss, io, this);
}

int wpl_block_if::run(wpl_state *state, wpl_value *final_result) {
	int ret = WPL_OP_NO_RETURN;

	wpl_block_if_state *block_state =
		(wpl_block_if_state*) state;

	if (flag == F_ELSE || check_run(block_state)) {
		ret = wpl_block_intermediate::run(state, final_result);
	}
	else if (next_else_if.get()) {
		ret = block_state->run_next_else_if (next_else_if.get(), final_result);
	}

	return ret;
}

void wpl_block_if::parse_value(wpl_namespace *ns) {
	set_parent_namespace (ns);

	wpl_block *block = new wpl_block();
	set_runable(block);

	if (flag == F_ELSE) {
		/* No need for intermediate NS in else blocks */
		block->set_parent_namespace(ns);
	}
	else {
		block->set_parent_namespace(this);

		wpl_expression_par_enclosed *exp = new wpl_expression_par_enclosed();
		run_condition.reset(exp);

		ignore_whitespace();
		if (!ignore_letter ('(')) {
			THROW_ELEMENT_EXCEPTION("Expected '(' in if definition");
		}

		exp->insert_fake_open_par();
		exp->load_position(get_position());
		exp->parse_value(this);
		load_position(exp->get_position());
	}

	ignore_blockstart();

	block->load_position(get_position());
	block->parse_value(block);
	load_position(block->get_position());

	ignore_whitespace();

	if (ignore_string (wpl_blockname_else_if)) {
		next_else_if.reset(new wpl_block_if(F_ELSE_IF));
	}
	else if (ignore_string (wpl_blockname_else)) {
		next_else_if.reset(new wpl_block_if(F_ELSE));
	}
	else {
		return;
	}

	next_else_if->load_position(get_position());
	next_else_if->parse_value(ns);
	load_position(next_else_if->get_position());
}
