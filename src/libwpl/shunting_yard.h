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

#include "shunting_yard_carrier.h"
#include "exp_deque.h"
#include "exception.h"
#include "debug.h"

#include <stack>
#include <list>

using namespace std;

typedef struct wpl_operator_struct wpl_operator_struct;
class wpl_value;

class shunting_yard_stack {
	private:
	stack<const wpl_operator_struct*> my_stack;

	protected:
	void stack_push(const wpl_operator_struct *e) {
		my_stack.push(e);
	}
	const wpl_operator_struct *stack_pop() {
		if (my_stack.size() > 0) {
			const wpl_operator_struct *ret = my_stack.top();
			my_stack.pop();
			return ret;
		}
		return NULL;
	}
};

class shunting_yard_queue {
	private:
	list<shunting_yard_carrier> my_list;

	protected:
	void out_add (wpl_value *value) {
		my_list.push_back(shunting_yard_carrier(value));
	}
	void out_add (const wpl_operator_struct *op) {
		my_list.push_back(shunting_yard_carrier(op));
	}

	public:
	int count() {
		return my_list.size();
	}

	void dump();

	const list<shunting_yard_carrier> &get_list() {
		return my_list;
	}
};

class shunting_yard : public shunting_yard_queue, public shunting_yard_stack {
	private:
	wpl_exp_deque<shunting_yard_carrier> run_stack_template;

	protected:

	const wpl_exp_deque<shunting_yard_carrier> &get_run_stack() {
		return run_stack_template;
	}

	int count() {
		return shunting_yard_queue::count();
	}

	shunting_yard *clone() const {
		return new shunting_yard(*this);
	}

	public:
	void finish();
	void prepare_operator (const wpl_operator_struct *op);
	void shunt_operator (const wpl_operator_struct *op);
	void shunt (wpl_value *value);
};

