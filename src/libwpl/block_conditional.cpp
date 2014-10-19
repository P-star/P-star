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

#include "block_conditional.h"
#include "parser.h"
#include "value_bool.h"

bool wpl_block_conditional::check_run(wpl_block_conditional_state *block_state) {
	wpl_value_bool return_value;
	return_value.set_do_finalize();

	int ret = block_state->run_condition(run_condition.get(), &return_value);
	return ret & WPL_OP_OK && return_value.get();
}

void wpl_block_conditional::find_and_parse_complete_type() {
	char buf[WPL_VARNAME_SIZE+1];

	ignore_whitespace();

	if (int len = search (WORD, 0, false)) {
		check_varname_length(len);
		get_string(buf, len);

		/* Don't check for incomplete types like struct */
		if (wpl_parseable_identifier *parseable = find_complete_type(buf)) {
			wpl_parser::parse_parseable_identifier(this, this, parseable);
		}
		else if (wpl_parseable_identifier *parseable = find_template_type(buf)) {
			wpl_parser::parse_parseable_identifier(this, this, parseable);
		}
		else {
			revert_string(len);
		}
	}
}
