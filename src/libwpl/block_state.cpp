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

#include "block_state.h"

#include "runable.h"
/*
void wpl_block_state::register_child (wpl_runable *runable, int index) {
	if (index >= WPL_BLOCK_MAX_CHILDREN) {
		throw runtime_error("Too many children for wpl_block_state");
	}
	child_states[index].reset(runable->new_state());
}
*/

void wpl_block_state::clear_child_states() {
	for (int i = 0; i < WPL_BLOCK_MAX_CHILDREN; i++) {
		if (child_states[i].get() == nullptr) {
			break;
		}
		child_states[i].reset(nullptr);
	}
}

int wpl_block_state::run_child (wpl_runable *child, int index, wpl_value *final_result) {
	if (index >= WPL_BLOCK_MAX_CHILDREN) {
		throw runtime_error("Child index out of range in wpl_block_state");
	}
	if (child_states[index].get() == nullptr) {
		child_states[index].reset(child->new_state(this, io));
	}
	return child->run(child_states[index].get(), final_result);
}

int wpl_block_state::run_run_condition (wpl_runable *runable, wpl_value *final_result) {
	if (run_condition_state.get() == nullptr) {
		run_condition_state.reset(runable->new_state(this, io));
	}
	return runable->run(run_condition_state.get(), final_result);
}

int wpl_block_state::run_init (wpl_runable *runable, wpl_value *final_result) {
	if (init_state.get() == nullptr) {
		init_state.reset(runable->new_state(this, io));
	}
	return runable->run(init_state.get(), final_result);
}

int wpl_block_state::run_increment (wpl_runable *runable, wpl_value *final_result) {
	if (increment_state.get() == nullptr) {
		increment_state.reset(runable->new_state(this, io));
	}
	return runable->run(increment_state.get(), final_result);
}

int wpl_block_state::run_next_else_if (wpl_runable *runable, wpl_value *final_result) {
	if (next_else_if_state.get() == nullptr) {
		next_else_if_state.reset(runable->new_state(this, io));
	}
	return runable->run(next_else_if_state.get(), final_result);
}
