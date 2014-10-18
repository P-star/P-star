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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with P*. If not, see <http://www.gnu.org/licenses/>.

-------------------------------------------------------------

*/

#include "operator_special.h"
#include "operator.h"
#include "operator_types.h"
#include "shuntable.h"
#include "runable_operator.h"
#include "range.h"

class wpl_expression_state;
class wpl_value;

#include <stdexcept>
#include <cstring>

wpl_runable_operator *wpl_operator_struct::new_runable (
		wpl_expression_state *exp_state,
		wpl_value *lhs,
		wpl_value *rhs
) const {
	throw runtime_error("Runable not provided by this operator");
}

const wpl_operator_struct OP_PAR_OPEN("(", 0, WPL_OP_F_NONE, 1);
const wpl_operator_struct OP_PAR_CLOSE(")", 0, WPL_OP_F_NONE, 1);

const wpl_operator_struct OP_BRACK_OPEN("[", 0, WPL_OP_F_NONE, 1);
const wpl_operator_struct OP_BRACK_CLOSE("]", 0, WPL_OP_F_NONE, 1);

const wpl_operator_struct OP_FUNCTION_CALL("()", 1, WPL_OP_F_LEFT_BOTH_BIG|WPL_OP_F_OPTIONAL_LHS|WPL_OP_F_OPTIONAL_RHS, 2);
const wpl_operator_struct OP_ARRAY_SUBSCRIPTING("[]", 1, WPL_OP_F_LEFT_BOTH_BIG, 2);
const wpl_operator_struct OP_ELEMENT("->", 1, WPL_OP_F_LEFT_BOTH_BIG, 2);

const wpl_operator_struct OP_INDIRECTION("*", 1, WPL_OP_F_RIGHT_ONE_BIG, 1);
const wpl_operator_struct OP_POINTERTO("&", 1, WPL_OP_F_RIGHT_ONE_BIG, 1);

const wpl_operator_struct OP_COUNT("@", 2, WPL_OP_F_RIGHT_ONE_BIG, 1);
const wpl_operator_struct OP_IS_EMPTY("isempty", 2, WPL_OP_F_RIGHT_ONE_BIG, 7);
const wpl_operator_struct OP_DEFINED("defined", 2, WPL_OP_F_RIGHT_ONE_BIG, 7);
const wpl_operator_struct OP_KEYS("keys", 2, WPL_OP_F_RIGHT_ONE_BIG, 4);

const wpl_operator_struct OP_INC_SUFFIX("++", 2, WPL_OP_F_LEFT_ONE_BIG, 2);
const wpl_operator_struct OP_DEC_SUFFIX("--", 2, WPL_OP_F_LEFT_ONE_BIG, 2);
const wpl_operator_struct OP_INC_PREFIX("++", 3, WPL_OP_F_RIGHT_ONE_BIG, 2);
const wpl_operator_struct OP_DEC_PREFIX("--", 3, WPL_OP_F_RIGHT_ONE_BIG, 2);
const wpl_operator_struct OP_BITWISE_NOT("~", 3, WPL_OP_F_RIGHT_ONE_BIG, 1);
const wpl_operator_struct OP_LOGIC_NOT("!", 3, WPL_OP_F_RIGHT_ONE_BIG, 1);

const wpl_operator_struct OP_PATTERN_EQ("=~", 4, WPL_OP_F_LEFT_BOTH_BIG, 2);
const wpl_operator_struct OP_PATTERN_NOT_EQ("!~", 4, WPL_OP_F_LEFT_BOTH_BIG, 2);

const wpl_operator_struct OP_MUL("*", 5, WPL_OP_F_LEFT_BOTH_BIG, 1);
const wpl_operator_struct OP_DIV("/", 5, WPL_OP_F_LEFT_BOTH_BIG, 1);
const wpl_operator_struct OP_MOD("%", 5, WPL_OP_F_LEFT_BOTH_BIG, 1);

const wpl_operator_struct OP_SUM("+", 6, WPL_OP_F_LEFT_BOTH_BIG, 1);
const wpl_operator_struct OP_SUB("-", 6, WPL_OP_F_LEFT_BOTH_BIG, 1);
const wpl_operator_struct OP_CONCAT(".", 6, WPL_OP_F_LEFT_BOTH_BIG, 1);

const wpl_operator_struct OP_SH_LEFT("<<", 7, WPL_OP_F_LEFT_BOTH_BIG, 2);
const wpl_operator_struct OP_SH_RIGHT(">>", 7, WPL_OP_F_LEFT_BOTH_BIG, 2);

const wpl_operator_struct OP_BITWISE_AND("&", 8, WPL_OP_F_LEFT_BOTH_BIG, 1);
const wpl_operator_struct OP_BITWISE_XOR("^", 9, WPL_OP_F_LEFT_BOTH_BIG, 1);
const wpl_operator_struct OP_BITWISE_OR("|", 10, WPL_OP_F_LEFT_BOTH_BIG, 1);

const wpl_operator_struct OP_LTEQ("<=", 11, WPL_OP_F_LEFT_BOTH_BIG, 2);
const wpl_operator_struct OP_GTEQ(">=", 11, WPL_OP_F_LEFT_BOTH_BIG, 2);
const wpl_operator_struct OP_LT("<", 11, WPL_OP_F_LEFT_BOTH_BIG, 1);
const wpl_operator_struct OP_GT(">", 11, WPL_OP_F_LEFT_BOTH_BIG, 1);

const wpl_operator_struct OP_EQ("==", 12, WPL_OP_F_LEFT_BOTH_BIG, 2);
const wpl_operator_struct OP_NOT_EQ("!=", 12, WPL_OP_F_LEFT_BOTH_BIG, 2);

const wpl_operator_struct OP_LOGIC_AND("&&", 13, WPL_OP_F_LEFT_BOTH_BIG, 2);
const wpl_operator_struct OP_LOGIC_OR("||", 14, WPL_OP_F_LEFT_BOTH_BIG, 2);

const wpl_operator_struct OP_REPLACE_DISCARD("==>", 15, WPL_OP_F_RIGHT_BOTH_BIG, 3);
const wpl_operator_struct OP_SAVE_DISCARD("=>", 15, WPL_OP_F_RIGHT_BOTH_BIG, 2);

const wpl_operator_struct OP_ASSIGN("=", 16, WPL_OP_F_RIGHT_BOTH_BIG, 1);
const wpl_operator_struct OP_ASSIGN_SUM("+=", 16, WPL_OP_F_RIGHT_BOTH_BIG, 2);
const wpl_operator_struct OP_ASSIGN_SUB("-=", 16, WPL_OP_F_RIGHT_BOTH_BIG, 2);
const wpl_operator_struct OP_ASSIGN_MUL("*=", 16, WPL_OP_F_RIGHT_BOTH_BIG, 2);
const wpl_operator_struct OP_ASSIGN_DIV("/=", 16, WPL_OP_F_RIGHT_BOTH_BIG, 2);
const wpl_operator_struct OP_ASSIGN_MOD("%=", 16, WPL_OP_F_RIGHT_BOTH_BIG, 2);
const wpl_operator_struct OP_ASSIGN_AND("&=", 16, WPL_OP_F_RIGHT_BOTH_BIG, 2);
const wpl_operator_struct OP_ASSIGN_XOR("^=", 16, WPL_OP_F_RIGHT_BOTH_BIG, 2);
const wpl_operator_struct OP_ASSIGN_OR("|=", 16, WPL_OP_F_RIGHT_BOTH_BIG, 2);
const wpl_operator_struct OP_ASSIGN_SH_LEFT("<<=", 16, WPL_OP_F_RIGHT_BOTH_BIG, 3);
const wpl_operator_struct OP_ASSIGN_SH_RIGHT(">>=", 16, WPL_OP_F_RIGHT_BOTH_BIG, 3);
const wpl_operator_struct OP_ASSIGN_CONCAT(".=", 16, WPL_OP_F_RIGHT_BOTH_BIG, 2);

const wpl_operator_range_inclusive OP_RANGE_INCLUSIVE("..", 17, WPL_OP_F_LEFT_BOTH_BIG|WPL_OP_F_HAS_RUNABLE, 2);
const wpl_operator_range_exclusive OP_RANGE_EXCLUSIVE("...", 17, WPL_OP_F_LEFT_BOTH_BIG|WPL_OP_F_HAS_RUNABLE, 3);

const wpl_operator_struct OP_DISCARD(",", 18, WPL_OP_F_LEFT_ONE_BIG, 1);

const wpl_operator_struct OP_ECHO("echo", 19, WPL_OP_F_RIGHT_ONE_BIG, 4);
const wpl_operator_struct OP_ERRCHO("errcho", 19, WPL_OP_F_RIGHT_ONE_BIG, 6);

const wpl_operator_struct OP_BREAK_OP("break",20, WPL_OP_F_NONE, 5);
const wpl_operator_struct OP_RETURN_OP("return",20, WPL_OP_F_RIGHT_ONE_BIG, 6);
const wpl_operator_struct OP_BLOCK_RETURN_OP("block_return",20, WPL_OP_F_RIGHT_ONE_BIG, 12);

static const struct wpl_operator_struct *operators[] = {
	&OP_ARRAY_SUBSCRIPTING,
	&OP_MUL, // Must be above ELEMENT
	&OP_ELEMENT,

	// These three are all have dots, most dots at the top
	&OP_RANGE_EXCLUSIVE,
	&OP_RANGE_INCLUSIVE,
	&OP_CONCAT,

	&OP_REPLACE_DISCARD,
	&OP_SAVE_DISCARD,

	&OP_INC_SUFFIX,
	&OP_ASSIGN,
	&OP_LTEQ,
	&OP_GTEQ,
	&OP_LT,
	&OP_GT,
	&OP_EQ,
	&OP_PATTERN_EQ,
	&OP_SUM,
	&OP_SUB,
	&OP_INC_PREFIX,
	&OP_ASSIGN_SUM,
	&OP_NOT_EQ,
	&OP_PATTERN_NOT_EQ,
	&OP_LOGIC_AND,
	&OP_LOGIC_OR,
	&OP_LOGIC_NOT,
	&OP_DISCARD,
	&OP_BREAK_OP,
	&OP_IS_EMPTY,
	&OP_RETURN_OP,
	&OP_ECHO,
	&OP_ERRCHO,
	&OP_INDIRECTION,
	&OP_POINTERTO,
	&OP_COUNT,
	&OP_DEFINED,
	&OP_KEYS,

	&OP_DEC_SUFFIX,
	&OP_DEC_PREFIX,
	&OP_ASSIGN_SUB,
	&OP_ASSIGN_MUL,
	&OP_ASSIGN_DIV,
	&OP_ASSIGN_MOD,
	&OP_ASSIGN_CONCAT,
	&OP_DIV,
	&OP_MOD,
	&OP_ASSIGN_SH_LEFT,
	&OP_ASSIGN_SH_RIGHT,
	&OP_SH_LEFT,
	&OP_SH_RIGHT,
	&OP_BITWISE_NOT,
	&OP_ASSIGN_AND,
	&OP_ASSIGN_XOR,
	&OP_ASSIGN_OR,
	&OP_BITWISE_AND,
	&OP_BITWISE_XOR,
	&OP_BITWISE_OR,
	&OP_BLOCK_RETURN_OP,
	NULL
};

const struct wpl_operator_struct *wpl_get_operator(const char *name, int flags) {
	int i;
	const struct wpl_operator_struct *op;

	for (i = 0, op = operators[0]; op != NULL; i++, op = operators[i]) {
		if ((flags & op->flags) && (strcmp(name, op->name) == 0)) {
			return op;
		}
	}
	return NULL;
}
