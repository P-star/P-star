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

#include "value_holder.h"

#include <string>

class wpl_function;
class wpl_expression_state;
class wpl_namespace_session;

using namespace std;

class wpl_value_function_ptr : public wpl_value {
	private:
	wpl_function *function;
	wpl_namespace_session *nss_this;

	int saved_discard_pos;
	bool did_run;

	public:
	PRIMITIVE_TYPEATTR(function_ptr)

	wpl_value_function_ptr *clone() const {
		return new wpl_value_function_ptr(*this); 
	};

	wpl_value_function_ptr(wpl_function *function) {
		this->function = function;
		this->nss_this = NULL;
		this->saved_discard_pos = 0;
	}

	wpl_value_function_ptr (
			wpl_function *function,
			wpl_namespace_session *nss_this,
			wpl_expression_state *exp_state
	);

	~wpl_value_function_ptr() {
#ifdef WPL_DEBUG_DESTRUCTION
		DBG("V (" << this << "): Destructing function pointer\n");
#endif
	}

	int do_operator (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const struct wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
	);
};

