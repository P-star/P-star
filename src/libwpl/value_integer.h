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

#include "value_number.h"
#include "operator.h"
#include "value.h"

template<typename A> class wpl_value_integers : public wpl_value_number<A> {
	private:

	int inc_suffix() {RESULT = (VALUE)++; return WPL_OP_OK; }
	int dec_suffix() {RESULT = (VALUE)--; return WPL_OP_OK; }
	int inc_prefix() {RESULT = ++(VALUE); return WPL_OP_OK; }
	int dec_prefix() {RESULT = --(VALUE); return WPL_OP_OK; }

	int mod() {RESULT = LHS % RHS; return WPL_OP_OK; }
	int assign_mod() {RESULT = LHS %= RHS; return WPL_OP_OK|WPL_OP_ASSIGN; }

	int bitwise_not() {RESULT = ~RHS; return WPL_OP_OK; }

	int sh_left() {RESULT = LHS << RHS; return WPL_OP_OK; }
	int sh_right() {RESULT = LHS >> RHS; return WPL_OP_OK; }

	int assign_sh_left() {RESULT = LHS <<= RHS; return WPL_OP_OK|WPL_OP_ASSIGN; }
	int assign_sh_right() {RESULT = LHS >>= RHS; return WPL_OP_OK|WPL_OP_ASSIGN; }

	int bitwise_and() {RESULT = LHS & RHS; return WPL_OP_OK; }
	int bitwise_or() {RESULT = LHS | RHS; return WPL_OP_OK; }
	int bitwise_xor() {RESULT = LHS ^ RHS; return WPL_OP_OK; }

	int assign_and() {RESULT = LHS &= RHS; return WPL_OP_OK|WPL_OP_ASSIGN; }
	int assign_or() {RESULT = LHS |= RHS; return WPL_OP_OK|WPL_OP_ASSIGN; }
	int assign_xor() {RESULT = LHS ^= RHS; return WPL_OP_OK|WPL_OP_ASSIGN; }

	protected:
	virtual int __do_operator (const struct wpl_operator_struct *op) {
		int res;
		if ((res = wpl_value_number<A>::__do_operator(op)) != WPL_OP_UNKNOWN) {
			return res;
		}

		CALL_OP(OP_INC_SUFFIX, inc_suffix)
		CALL_OP(OP_DEC_SUFFIX, dec_suffix)
		CALL_OP(OP_INC_PREFIX, inc_prefix)
		CALL_OP(OP_DEC_PREFIX, dec_prefix)
		CALL_OP(OP_MOD,mod)
		CALL_OP(OP_ASSIGN_MOD, assign_mod)

		CALL_OP(OP_BITWISE_NOT, bitwise_not)

		CALL_OP(OP_BITWISE_AND, bitwise_and)
		CALL_OP(OP_BITWISE_OR, bitwise_or)
		CALL_OP(OP_BITWISE_XOR, bitwise_xor)

		CALL_OP(OP_ASSIGN_SH_LEFT, assign_sh_left)
		CALL_OP(OP_ASSIGN_SH_RIGHT, assign_sh_right)

		CALL_OP(OP_SH_RIGHT, sh_right)
		CALL_OP(OP_SH_LEFT, sh_left)

		CALL_OP(OP_ASSIGN_AND, assign_and)
		CALL_OP(OP_ASSIGN_OR, assign_or)
		CALL_OP(OP_ASSIGN_XOR, assign_xor)

		return WPL_OP_UNKNOWN;
	}
};
