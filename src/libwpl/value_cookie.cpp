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

#include "value_cookie.h"
#include "operator.h"
#include "value_string.h"
#include "value_array.h"
#include "matcher.h"

#include <stdlib.h>
#include <memory>
#include <cstring>
#include <map>

const wpl_type_array wpl_value_cookie::type_array("GET array");
const wpl_type_bool wpl_value_cookie::type_bool("GET bool");
const wpl_type_array_instance wpl_value_cookie::type_complete_array(&type_array, &type_bool);

void wpl_value_cookie::parse() {
}

int wpl_value_cookie::do_operator (
		wpl_expression_state *exp_state,
		wpl_value *final_result,
		const wpl_operator_struct *op,
		wpl_value *lhs,
		wpl_value *rhs
) {
	if (!did_parse) {
		parse();
		did_parse = true;
	}
	return hash.do_operator(
			exp_state,
			final_result,
			op,
			(lhs == this ? &hash : lhs),
			(rhs == this ? &hash : rhs)
			);
}
