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

#include "text_state.h"
#include "expression.h"
#include "text.h"
#include "expression_state.h"
#include "operator_types.h"

int wpl_text_state::run_expression(wpl_expression *exp, int index, wpl_value *final_result) {
	if (expression_states[index].get() == nullptr) {
		expression_states[index].reset(exp->new_state(nss, io));
	}
	return exp->run(expression_states[index].get(), final_result);
}

int wpl_text_state::run_text(wpl_text *text, int index, wpl_value *final_result, wpl_io &io) {
	if (text_states[index].get() == nullptr) {
		text_states[index].reset(text->new_state(nss, wpl_state::io));
	}
	return text->run(text_states[index].get(), final_result, io);
}


int wpl_text_state::run_text_output_json (
		wpl_text *text,
		int index,
		wpl_value *final_result,
		const set<wpl_value*> &vars
) {
	if (text_states[index].get() == nullptr) {
		text_states[index].reset(text->new_state(nss, io));
	}
	return text->output_json(text_states[index].get(), vars, final_result);
}
