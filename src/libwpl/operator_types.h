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

#include "operator_special.h"

extern const wpl_operator_struct OP_PAR_OPEN;
extern const wpl_operator_struct OP_PAR_CLOSE;

extern const wpl_operator_struct OP_BRACK_OPEN;
extern const wpl_operator_struct OP_BRACK_CLOSE;

extern const wpl_operator_struct OP_FUNCTION_CALL;
extern const wpl_operator_struct OP_ARRAY_SUBSCRIPTING;
extern const wpl_operator_struct OP_ELEMENT;
extern const wpl_operator_struct OP_INDIRECTION;
extern const wpl_operator_struct OP_POINTERTO;
extern const wpl_operator_struct OP_COUNT;
extern const wpl_operator_struct OP_DEFINED;
extern const wpl_operator_struct OP_KEYS;

extern const wpl_operator_struct OP_IS_EMPTY;

extern const wpl_operator_struct OP_INC_SUFFIX;
extern const wpl_operator_struct OP_DEC_SUFFIX;
extern const wpl_operator_struct OP_INC_PREFIX;
extern const wpl_operator_struct OP_DEC_PREFIX;

extern const wpl_operator_struct OP_REPLACE_DISCARD;
extern const wpl_operator_struct OP_SAVE_DISCARD;

extern const wpl_operator_struct OP_ASSIGN_SUM;
extern const wpl_operator_struct OP_ASSIGN_SUB;
extern const wpl_operator_struct OP_ASSIGN_MUL;
extern const wpl_operator_struct OP_ASSIGN_DIV;
extern const wpl_operator_struct OP_ASSIGN_MOD;
extern const wpl_operator_struct OP_ASSIGN_CONCAT;

extern const wpl_operator_range_inclusive OP_RANGE_INCLUSIVE;
extern const wpl_operator_range_exclusive OP_RANGE_EXCLUSIVE;

extern const wpl_operator_struct OP_SUM;
extern const wpl_operator_struct OP_SUB;
extern const wpl_operator_struct OP_MUL;
extern const wpl_operator_struct OP_DIV;
extern const wpl_operator_struct OP_MOD;
extern const wpl_operator_struct OP_CONCAT;

extern const wpl_operator_struct OP_ASSIGN_SH_LEFT;
extern const wpl_operator_struct OP_ASSIGN_SH_RIGHT;

extern const wpl_operator_struct OP_SH_LEFT;
extern const wpl_operator_struct OP_SH_RIGHT;

extern const wpl_operator_struct OP_LTEQ;
extern const wpl_operator_struct OP_GTEQ;
extern const wpl_operator_struct OP_LT;
extern const wpl_operator_struct OP_GT;

extern const wpl_operator_struct OP_BITWISE_NOT;

extern const wpl_operator_struct OP_LOGIC_AND;
extern const wpl_operator_struct OP_LOGIC_OR;

extern const wpl_operator_struct OP_ASSIGN_AND;
extern const wpl_operator_struct OP_ASSIGN_XOR;
extern const wpl_operator_struct OP_ASSIGN_OR;

extern const wpl_operator_struct OP_BITWISE_AND;
extern const wpl_operator_struct OP_BITWISE_XOR;
extern const wpl_operator_struct OP_BITWISE_OR;

extern const wpl_operator_struct OP_EQ;
extern const wpl_operator_struct OP_NOT_EQ;

extern const wpl_operator_struct OP_PATTERN_EQ;
extern const wpl_operator_struct OP_PATTERN_NOT_EQ;

extern const wpl_operator_struct OP_ASSIGN;
extern const wpl_operator_struct OP_LOGIC_NOT;

extern const wpl_operator_struct OP_DISCARD;

extern const wpl_operator_struct OP_BREAK_OP;
extern const wpl_operator_struct OP_RETURN_OP;
extern const wpl_operator_struct OP_BLOCK_RETURN_OP;

extern const wpl_operator_struct OP_ECHO;
extern const wpl_operator_struct OP_ERRCHO;

