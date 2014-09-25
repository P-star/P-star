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

#include "operator_special.h"
#include "range.h"

wpl_operator_range_inclusive::wpl_operator_range_inclusive (const char *op, int precedence, int flags, int length) :
	wpl_operator_struct (op, precedence, flags, length)
{}

wpl_runable_operator *wpl_operator_range_inclusive::new_runable (
		wpl_expression_state *exp_state,
		wpl_value *lhs,
		wpl_value *rhs
) const {
	return new wpl_range(exp_state, this, lhs, rhs);
}

wpl_operator_range_exclusive::wpl_operator_range_exclusive (const char *op, int precedence, int flags, int length) :
	wpl_operator_struct (op, precedence, flags, length)
{}

wpl_runable_operator *wpl_operator_range_exclusive::new_runable(
		wpl_expression_state *exp_state,
		wpl_value *lhs,
		wpl_value *rhs
) const {
	return new wpl_range(exp_state, this, lhs, rhs);
}

