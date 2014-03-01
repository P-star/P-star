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

#include "value_file.h"
#include "operator.h"
#include "value_int.h"
#include "value_string.h"

#include <cstring>
#include <sstream>

void wpl_value_file::set_weak (wpl_value *value) {
	/*
	   TODO
	   Support setting from string and int
	   */
	wpl_value_file *value_file = dynamic_cast<wpl_value_file*>(value);
	if (value_file == NULL) {
		ostringstream tmp;
		tmp << "Could not set FILE object to value of type " << value->get_type_name();
		throw runtime_error(tmp.str());
	}
	else {
		throw runtime_error("FILE weak set not implemented");
	}
}

string wpl_value_file::toString() {
	throw runtime_error("FILE toString() not implemented");
}

int wpl_value_file::do_operator (
		wpl_expression_state *exp_state,
		wpl_value *final_result,
		const wpl_operator_struct *op,
		wpl_value *lhs,
		wpl_value *rhs
) {
	throw runtime_error("FILE do_operator() not implemented");
	return WPL_OP_UNKNOWN;
}
