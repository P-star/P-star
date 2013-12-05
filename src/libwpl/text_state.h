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

#pragma once

#include "state.h"
#include "block_state.h"
#include "io.h"

#include <set>
#include <memory>
#include <sstream>
#include <unordered_map>

class wpl_text;
class wpl_expression;

class wpl_text_state : public wpl_state {
	protected:
	unordered_map<int,unique_ptr<wpl_state>> expression_states;
	unordered_map<int,unique_ptr<wpl_state>> text_states;

	public:
	wpl_text_state(wpl_namespace_session *nss, wpl_io *io, int children) :
		wpl_state(nss, io)
	{
		expression_states.reserve(children/2);
	}
	wpl_namespace_session *get_nss() {
		return nss;
	}
	wpl_state *get_exp_state(int index) {
		return expression_states[index].get();
	}
	int run_expression (
			wpl_expression *exp,
			int index,
			wpl_value *final_result
	);
	int run_text (
			wpl_text *text,
			int index,
			wpl_value *final_result,
			wpl_io &io
	);
	int run_text_output_json(
			wpl_text *text,
			int index,
			wpl_value *final_result,
			const set<wpl_value*> &vars
	);
};
