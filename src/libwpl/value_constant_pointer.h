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

#include "value.h"
#include "typenames.h"
#include "type_precedence.h"

using namespace std;

class wpl_value_constant_pointer : public wpl_value {
	private:
	wpl_value *value= NULL;

	public:
	int get_precedence() const { return wpl_type_precedence_constant_pointer; };
	const char *get_type_name() const { return wpl_typename_constant_pointer; }
	wpl_value *clone() const {
		return new wpl_value_constant_pointer (*this);
	};

	wpl_value *dereference() {
		return value;
	}

	int finalize_expression (wpl_expression_state *exp_state, wpl_value *last_value) override {
		if (last_value->get_flags() & wpl_value_is_constant) {
			value = last_value;
		}
		else {
			value = NULL;
		}
		return WPL_OP_OK;
	}
};

