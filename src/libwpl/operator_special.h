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

#pragma once

#include "operator.h"

class wpl_operator_range_inclusive : public wpl_operator_struct {
	public:
	wpl_operator_range_inclusive (const char *op, int precedence, int flags, int length);
	wpl_runable_operator *new_runable(wpl_expression_state *exp_state, wpl_value *lhs, wpl_value *rhs) const override;
};

class wpl_operator_range_exclusive : public wpl_operator_struct {
	public:
	wpl_operator_range_exclusive (const char *op, int precedence, int flags, int length);
	wpl_runable_operator *new_runable(wpl_expression_state *exp_state, wpl_value *lhs, wpl_value *rhs) const override;
};
