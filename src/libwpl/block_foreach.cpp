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

#include "value_bool.h"
#include "value_range.h"
#include "value_constant_pointer.h"
#include "block_foreach.h"
#include "expression.h"

#include <memory>

wpl_block_foreach::wpl_block_foreach() :
	exp_var(new wpl_expression()),
	exp_condition(new wpl_expression_par_enclosed())
{}

int wpl_block_foreach::run(wpl_state *state, wpl_value *final_result) {
	int ret = WPL_OP_NO_RETURN;
	wpl_block_state *block_state = (wpl_block_state*) state;

	wpl_value *var;
	wpl_value_constant_pointer constant_pointer;
	constant_pointer.set_do_finalize();
	if (!(ret = block_state->run_init(exp_var.get(), &constant_pointer)) & WPL_OP_OK) {
		return ret;
	}
	var = constant_pointer.dereference();

	if (var == NULL) {
		throw runtime_error("No constant found in foreach loop variable section");
	}

	wpl_value_range range(var);
	while ((ret = block_state->run_run_condition(exp_condition.get(), &range)) & WPL_OP_OK) {
		if (!(ret & WPL_OP_RANGE)) {
			break;
		}
		if (ret & (WPL_OP_RANGE_COMPLETE)) {
			ret &= ~(WPL_OP_RANGE_COMPLETE);
			break;
		}

		ret = wpl_block::run(block_state, final_result);

		if (ret & (WPL_OP_BREAK|WPL_OP_RETURN)) {
			ret &= ~(WPL_OP_BREAK);
			break;
		}
	}

	return ret;
}

void wpl_block_foreach::parse_value (wpl_namespace *ns) {
	set_parent_namespace(ns);

	ignore_whitespace();
	if (!ignore_letter ('(')) {
		THROW_ELEMENT_EXCEPTION("Expected '(' in foreach loop definition");
	}

	exp_var->load_position(get_position());
	exp_var->parse_value(ns);
	load_position(exp_var->get_position());

	exp_condition->insert_fake_open_par();
	exp_condition->load_position(get_position());
	exp_condition->parse_value(ns);
	load_position(exp_condition->get_position());

	ignore_blockstart();
	wpl_block::parse_value(this);
}
