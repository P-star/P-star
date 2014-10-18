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

#include "runable.h"

int wpl_block_conditional_state::run_condition (wpl_runable *runable, wpl_value *final_result) {
	if (condition_state.get() == nullptr) {
		condition_state.reset(runable->new_state(this, this, &get_io()));
	}
	return runable->run(condition_state.get(), final_result);
}

int wpl_block_foreach_state::run_condition (wpl_runable *runable, wpl_value *final_result, int loop_number) {
	if (condition_state.get() == nullptr) {
		condition_state.reset(runable->new_state(this, this, &get_io()));
	}
	return runable->run(condition_state.get(), final_result, loop_number);
}

int wpl_block_foreach_state::run_init (wpl_runable *runable, wpl_value *final_result) {
	if (init_state.get() == nullptr) {
		init_state.reset(runable->new_state(this, this, &get_io()));
	}
	return runable->run(init_state.get(), final_result);
}

int wpl_block_for_state::run_continue (wpl_runable *runable, wpl_value *final_result) {
	if (continue_state.get() == nullptr) {
		continue_state.reset(runable->new_state(this, this, &get_io()));
	}
	return runable->run(continue_state.get(), final_result);
}

int wpl_block_state::run_child (wpl_runable *child, int index, wpl_value *final_result) {
	if (index >= WPL_BLOCK_MAX_CHILDREN) {
		throw runtime_error("Child index out of range in wpl_block_state");
	}
	if (child_states[index].get() == nullptr) {
		child_states[index].reset(child->new_state(this, this, &get_io()));
	}
	return child->run(child_states[index].get(), final_result);
}

int wpl_block_if_state::run_next_else_if (wpl_runable *runable, wpl_value *final_result) {
	if (next_else_if_state.get() == nullptr) {
		next_else_if_state.reset(runable->new_state(this, this, &get_io()));
	}
	return runable->run(next_else_if_state.get(), final_result);
}

/**
 * @brief Reset all variables
 */
void wpl_block_state::reset() {
	if (++run_count > 1) {
		wpl_namespace_session::reset_variables();
	}
}
