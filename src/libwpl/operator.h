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

#include "operator_return_values.h"

class wpl_value;
class wpl_runable_operator;
class wpl_expression_state;

struct wpl_operator_struct {
	union {
		const char *op;
		const char *name;
	};

	const int precedence;
	const int flags;
	const int length;

	virtual wpl_runable_operator *new_runable (
			wpl_expression_state *exp_state,
			wpl_value *lhs,
			wpl_value *rhs
	) const;

	wpl_operator_struct(const char *op, int precedence, int flags, int length) :
		op(op),
		precedence(precedence),
		flags(flags),
		length(length)
	{}
};

#define WPL_OP_F_NONE		0

#define WPL_OP_F_ASSOC_LEFT	(1<<0)

#define WPL_OP_F_ASSOC_RIGHT	(1<<1)
#define WPL_OP_F_HAS_LHS	(1<<2)
#define WPL_OP_F_HAS_RHS	(1<<3)
#define WPL_OP_F_OPTIONAL_LHS	(1<<4)
#define WPL_OP_F_OPTIONAL_RHS	(1<<5)
#define WPL_OP_F_HAS_RUNABLE	(1<<6)

#define WPL_OP_F_LEFT_BOTH	(1<<7)
#define WPL_OP_F_LEFT_ONE	(1<<8)
#define WPL_OP_F_RIGHT_BOTH	(1<<9)
#define WPL_OP_F_RIGHT_ONE	(1<<10)

#define WPL_OP_F_ASSOC_ALL	(WPL_OP_F_ASSOC_LEFT|WPL_OP_F_ASSOC_RIGHT)

#define WPL_OP_F_HAS_BOTH	(WPL_OP_F_HAS_LHS|WPL_OP_F_HAS_RHS)

#define WPL_OP_F_LEFT_BOTH_BIG	(WPL_OP_F_LEFT_BOTH|WPL_OP_F_ASSOC_LEFT|WPL_OP_F_HAS_BOTH)
#define WPL_OP_F_LEFT_ONE_BIG	(WPL_OP_F_LEFT_ONE|WPL_OP_F_ASSOC_LEFT|WPL_OP_F_HAS_LHS)

#define WPL_OP_F_RIGHT_BOTH_BIG	(WPL_OP_F_RIGHT_BOTH|WPL_OP_F_ASSOC_RIGHT|WPL_OP_F_HAS_BOTH)
#define WPL_OP_F_RIGHT_ONE_BIG	(WPL_OP_F_RIGHT_ONE|WPL_OP_F_ASSOC_RIGHT|WPL_OP_F_HAS_RHS)
/*
#define WPL_OP_LEFT_PRECEDENCE(v) (	\
	(v->precedence == 2) ||		\
	(v->precedence == 3) ||		\
	(v->precedence == 16) ||	\
	(v->precedence >= 19)		\
)
*/
#define WPL_OP_IS_LEFT_ASSOC(v) \
	(v & WPL_OP_F_ASSOC_LEFT)
#define WPL_OP_IS_RIGHT_ASSOC(v) \
	(v & WPL_OP_F_ASSOC_RIGHT)
#define WPL_OP_IS_NO_ASSOC(v) \
	(!(v & (WPL_OP_F_ASSOC_LEFT|WPL_OP_F_ASSOC_RIGHT)))
#define WPL_OP_IS_LEFT_PAR(v) \
	(v==&OP_PAR_OPEN)
#define WPL_OP_IS_RIGHT_PAR(v) \
	(v==&OP_PAR_CLOSE)
#define WPL_OP_IS_COMMA(v) \
	(v->name[0] == ',')

#define WPL_OP_NAME_MAX_LENGTH 12

const struct wpl_operator_struct *wpl_get_operator (const char *name, int flags);

