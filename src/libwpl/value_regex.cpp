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

#include "value_regex.h"
#include "operator_return_values.h"

bool wpl_value_regex::do_pattern_match (wpl_expression_state *exp_state, string &subject) {
	/*
	   TODO
	   - add /g-modifier for multiple match
	   - push matches to discard queue
	   - add new left assoc operator => to push discard queue onto array
	   */

	bool do_global = false;
	bool result = false;

	boost::match_results<std::string::const_iterator> what; 
	boost::match_flag_type flags = boost::match_default;
	string::const_iterator start, end;

	start = subject.begin();
	end = subject.end();

	while (regex_search(start, end, what, my_regex, flags)) {
		result = true;

		if (!do_global) {
			break;
		}
	}

	return result;
}

int wpl_value_regex::do_operator (
		wpl_expression_state *exp_state,
		wpl_value *final_result,
		const wpl_operator_struct *op,
		wpl_value *lhs,
		wpl_value *rhs
		) {
	return WPL_OP_UNKNOWN;
}


