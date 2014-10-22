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

#include "text_state.h"
#include "value_array.h"
#include "runable.h"
#include "text.h"
#include "operator_types.h"

bool wpl_text_state::has_var(const string name) {
	for (int i = 0; i < vars->size(); i++) {
		const wpl_value *value = vars->get_readonly(i);
		if (value->toString() == name) {
			return true;
		}
	}
	return false;
}

wpl_state *wpl_text_state::get_child_state(wpl_runable *runable, int index) {
	wpl_state *runable_state;
	try {
		if (runable_state = child_states.at(index).get()) {
			return runable_state;
		}
	}
	catch (const std::out_of_range &e) {
		throw runtime_error("BUG: Size of text children state array not correctly set");
	}

	child_states[index].reset(runable->new_state(this, get_nss(), &get_io()));

	return child_states[index].get();
}

int wpl_text_state::run_runable(wpl_runable *runable, int index, wpl_value *final_result) {
	wpl_state *runable_state = get_child_state(runable, index);
	return runable->run(runable_state, final_result);
}

int wpl_text_state::run_text(wpl_text *text, int index, wpl_value *final_result, wpl_io &io) {
	wpl_state *text_state = get_child_state(text, index);
	return text->run(text_state, final_result);
}

int wpl_text_state::run_text_output_json (
		wpl_text *text,
		int index,
		wpl_value *final_result
) {
	return text->output_json(get_child_state(text, index), get_vars(), final_result);
}
