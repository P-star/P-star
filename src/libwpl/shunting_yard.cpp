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

#include "operator_types.h"
#include "shunting_yard.h"
#include "exception.h"
#include "expression.h"

void shunting_yard_queue::dump() {
	cout << "\n\n-----------------\n";
	for (shunting_yard_carrier &carrier : my_list) {
		if (carrier.op) {
			cout << carrier.op->name << " ";
		}
		else {
			cout << "{" << carrier.value->get_type_name() << "}";
			carrier.value->dump();
		}
	}
	cout << "\n";
}

void shunting_yard::finish () {
	const wpl_operator_struct *op;
	while (op = stack_pop()) {
		if (WPL_OP_IS_LEFT_PAR (op)) {
			throw runtime_error("shunting_yard::finish(): Mismatch parantheses in expression");
		}
		out_add(op);
	}

	run_stack_template.clear();
	list<shunting_yard_carrier>::const_reverse_iterator it = get_list().rbegin();
	for (; it != get_list().rend(); it++) {
		run_stack_template.push(*it);
	}

	if (get_list().size() > WPL_EXPRESSION_MAX) {
		throw runtime_error("Expression length limit exceed");
	}
}

void shunting_yard::shunt (wpl_value *value) {
	out_add(value);
}

void shunting_yard::prepare_operator (const wpl_operator_struct *new_op) {
	const wpl_operator_struct *stack_op;

	while (stack_op = stack_pop()) {
		if (!WPL_OP_IS_LEFT_PAR(stack_op) && (new_op->precedence > stack_op->precedence)) {
			out_add(stack_op);
		}
		else if (!WPL_OP_IS_LEFT_PAR(stack_op) &&
			(	(WPL_OP_IS_LEFT_ASSOC(new_op->flags) && (new_op->precedence >= stack_op->precedence)) ||
				(new_op->precedence > stack_op->precedence)
			)
		) {
			out_add(stack_op);
		}
		else {
			stack_push(stack_op);
			break;
		}
	}
}

void shunting_yard::shunt_operator (const wpl_operator_struct *new_op) {
	const wpl_operator_struct *stack_op;

	if (new_op == &OP_DISCARD) {
		while (stack_op = stack_pop()) {
			if (WPL_OP_IS_LEFT_PAR(stack_op)) {
				stack_push(stack_op);
				break;
			}
			else {
				out_add(stack_op);
			}
		}
		out_add(new_op);
	}
	else if (WPL_OP_IS_LEFT_PAR(new_op)) {
		stack_push(new_op);
	}
	else if (WPL_OP_IS_RIGHT_PAR(new_op)) {
		while (stack_op = stack_pop()) {
			if (WPL_OP_IS_LEFT_PAR(stack_op)) {
				break;
			}
			else {
				out_add(stack_op);
			}
		}
		if (stack_op == NULL) {
			throw runtime_error("shunting_yard::add_operator(): Mismatch parantheses in expression");
		}
	}
	else {
		while (stack_op = stack_pop()) {
			if (!WPL_OP_IS_LEFT_PAR(stack_op) && (new_op->precedence > stack_op->precedence)) {
				out_add(stack_op);
			}
			else if (!WPL_OP_IS_LEFT_PAR(stack_op) &&
				(	(WPL_OP_IS_LEFT_ASSOC(new_op->flags) && (new_op->precedence >= stack_op->precedence)) ||
					(new_op->precedence > stack_op->precedence)
				)
			) {
				out_add(stack_op);
			}
			else {
				stack_push(stack_op);
				break;
			}
		}
		stack_push(new_op);
	}
}
