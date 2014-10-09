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

#include "expression_state.h"
#include "operator_types.h"
#include "value_holder_macros.h"
#include "value.h"
#include "types.h"

template<typename A> class wpl_value_holder : public wpl_value {
	private:
	A lhs_tmp;
	A rhs_tmp;

	protected:
	int discard_op() {
		result = *lhs_value;
		return (WPL_OP_OK|WPL_OP_DISCARD);
	}
	int return_op() {
		result = *rhs_value;
		return (WPL_OP_OK|WPL_OP_RETURN);
	}
	int block_return_op() {
		result = *rhs_value;
		return (WPL_OP_OK|WPL_OP_BLOCK_RETURN);
	}

	A value;

	bool result_logic;
	A result;
	A *lhs_value;
	A *rhs_value;

	void set_lhs_value (wpl_value *lhs, A value_lhs
	) {
		lhs_tmp = value_lhs;
		lhs_value = (lhs == (wpl_value*) this ? &value : &lhs_tmp);
	}

	void set_rhs_value (wpl_value *rhs, A value_rhs
	) {
		rhs_tmp = value_rhs;
		rhs_value = (rhs == (wpl_value*) this ? &value : &rhs_tmp);
	}

	void set_values_to_myself() {
		lhs_value = &result;
		rhs_value = &result;
	}

	virtual int __do_operator (const struct wpl_operator_struct *op) {
		CALL_OP(OP_DISCARD, discard_op)
		CALL_OP(OP_RETURN_OP, return_op)
		CALL_OP(OP_BLOCK_RETURN_OP, block_return_op)
		return WPL_OP_UNKNOWN;
	}

	public:
	virtual ~wpl_value_holder() {}
	A get () { return value; }
	void set (A value) {
		this->value = value;
	}
};

#include "value_pointer.h"
#include "value_bool.h"
