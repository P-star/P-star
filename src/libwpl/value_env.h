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

#include "value_holder.h"
#include "exception.h"

class wpl_operator_struct;

class wpl_value_env : public wpl_value {
	protected:

	public:
	PRIMITIVE_TYPEINFO(env)
	wpl_value_env(int dummy) 
	{}
	wpl_value_env *clone() const { return new wpl_value_env(0); };
	wpl_value_env *clone_empty() const { return new wpl_value_env(0); };

	int do_operator (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
	);
};
