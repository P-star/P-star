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

#define OP_DEC(name) \
	extern const struct wpl_operator_struct name;

OP_DEC(OP_PAR_OPEN)
OP_DEC(OP_PAR_CLOSE)

OP_DEC(OP_BRACK_OPEN)
OP_DEC(OP_BRACK_CLOSE)

OP_DEC(OP_FUNCTION_CALL)
OP_DEC(OP_ARRAY_SUBSCRIPTING)
OP_DEC(OP_ELEMENT)
OP_DEC(OP_INDIRECTION)
OP_DEC(OP_COUNT)

OP_DEC(OP_IS_EMPTY)

OP_DEC(OP_INC_SUFFIX)
OP_DEC(OP_DEC_SUFFIX)
OP_DEC(OP_INC_PREFIX)
OP_DEC(OP_DEC_PREFIX)

OP_DEC(OP_SAVE_DISCARD)

OP_DEC(OP_ASSIGN_SUM)
OP_DEC(OP_ASSIGN_SUB)
OP_DEC(OP_ASSIGN_MUL)
OP_DEC(OP_ASSIGN_DIV)
OP_DEC(OP_ASSIGN_MOD)
OP_DEC(OP_ASSIGN_CONCAT)

OP_DEC(OP_SUM)
OP_DEC(OP_SUB)
OP_DEC(OP_MUL)
OP_DEC(OP_DIV)
OP_DEC(OP_MOD)
OP_DEC(OP_CONCAT)

OP_DEC(OP_ASSIGN_SH_LEFT)
OP_DEC(OP_ASSIGN_SH_RIGHT)

OP_DEC(OP_SH_LEFT)
OP_DEC(OP_SH_RIGHT)

OP_DEC(OP_LTEQ)
OP_DEC(OP_GTEQ)
OP_DEC(OP_LT)
OP_DEC(OP_GT)

OP_DEC(OP_BITWISE_NOT)

OP_DEC(OP_LOGIC_AND)
OP_DEC(OP_LOGIC_OR)

OP_DEC(OP_ASSIGN_AND)
OP_DEC(OP_ASSIGN_XOR)
OP_DEC(OP_ASSIGN_OR)

OP_DEC(OP_BITWISE_AND)
OP_DEC(OP_BITWISE_XOR)
OP_DEC(OP_BITWISE_OR)

OP_DEC(OP_EQ)
OP_DEC(OP_NOT_EQ)

OP_DEC(OP_PATTERN_EQ)
OP_DEC(OP_PATTERN_NOT_EQ)

OP_DEC(OP_ASSIGN)
OP_DEC(OP_LOGIC_NOT)

OP_DEC(OP_RANGE_INCLUSIVE)
OP_DEC(OP_RANGE_EXCLUSIVE)

OP_DEC(OP_DISCARD)

OP_DEC(OP_BREAK_OP)
OP_DEC(OP_RETURN_OP)
OP_DEC(OP_BLOCK_RETURN_OP)

OP_DEC(OP_ECHO)
OP_DEC(OP_ERRCHO)

