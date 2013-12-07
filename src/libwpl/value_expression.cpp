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

/*#include "value_expression.h"
#include "value_wrapper.h"
#include "expression.h"

wpl_value_expression::~wpl_value_expression() {
	delete my_expression;
}

int wpl_value_expression::run (wpl_expression_state *exp_state, wpl_value **final_result) {
	wpl_value_wrapper *result = new wpl_value_wrapper();
	*final_result = result;
	return (my_expression->run(exp_state, result) | WPL_OP_RETURN_REFERENCE);
}

void wpl_value_expression::dump() {
	my_expression->dump();
}

*/
