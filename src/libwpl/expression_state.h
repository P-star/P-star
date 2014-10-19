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

#include "state.h"
#include "block_state.h"
#include "exp_deque.h"
#include "shunting_yard_carrier.h"

#include <unordered_map>

using namespace std;

class wpl_namespace_session;
class wpl_runable;

class wpl_expression_child_state {
	unique_ptr<wpl_state> state;
	void *unique;
	public:
	wpl_expression_child_state() : state() {
		unique = NULL;
	}
	inline bool validate (void *test) {
		return (state.get() != nullptr && test == unique);
	}
	inline void set (wpl_state *state, void *unique) {
		this->state.reset(state);
		this->unique = unique;
	}
	inline wpl_state *get() {
		return state.get();
	}
};

class wpl_expression_state : public wpl_state {
	private:

	wpl_exp_deque<shunting_yard_carrier> run_stack;
	wpl_exp_deque<wpl_value*> wait_stack;
	wpl_exp_deque<wpl_value*> discard_chain;

	vector<wpl_expression_child_state> child_states;

	void optimize();
	int loop_number = 0;
	int flags = 0;

	public:
	static const int CTX_NONE = 0;
	static const int CTX_LOOP = 1;

	wpl_expression_state (
			wpl_state *parent,
			wpl_namespace_session *nss,
			wpl_io *io,
			const wpl_exp_deque<shunting_yard_carrier> &my_list
			) :
		wpl_state(parent, nss, io),
		run_stack(my_list),
		wait_stack(),
		discard_chain(),
		child_states(run_stack.size())
	{
		optimize();
		run_stack.save_pos();
	}

	// Used for internal dummy states where we have no real expression in the program
	wpl_expression_state (const wpl_state *state) :
		wpl_state(*state),
                run_stack(),
                wait_stack(),
                discard_chain(),
                child_states()
	{
		run_stack.save_pos();
	}

	wpl_variable *find_variable (const char *name) {
		return get_nss()->find_variable(name, WPL_NSS_CTX_SELF);
	}

	wpl_state *get_child_state(int index) {
		return child_states[index].get();
	}

	// RUN STACK
	void revert() {
		run_stack.revert_pos();
		wait_stack.clear();
		discard_chain.clear();
	}

	void replace (int index, shunting_yard_carrier &new_carrier) {
		run_stack.replace (index, new_carrier);
	}

	int pos() {
		return run_stack.pos();
	}

	bool empty() {
		return run_stack.empty();
	}

	shunting_yard_carrier &top() {
		return run_stack.top();
	}

	void pop() {
		run_stack.pop();
	}

	void unpop() {
		run_stack.unpop();
	}

	void push_extra(shunting_yard_carrier ca) {
		run_stack.push_front(ca);
		run_stack.save_pos();

		child_states.clear();
		child_states.resize(run_stack.size());
	}

	void push_extra(const wpl_operator_struct *op) {
		push_extra(shunting_yard_carrier(op));
	}

	void push_extra(wpl_value *value) {
		push_extra(shunting_yard_carrier(value));
	}

	wpl_exp_deque<shunting_yard_carrier> &get_stack() {
		return run_stack;
	}

	// WAITING OPERANDS QUEUE
	void push_waiting(wpl_value *value) {
		wait_stack.push(value);
	}

	void pop_waiting() {
		wait_stack.pop();
	}

	wpl_value *top_waiting() {
		return wait_stack.top();
	}

	bool empty_waiting() {
		return wait_stack.empty();
	}

	// DISCARD CHAIN
	wpl_exp_deque<wpl_value*> &get_discard() {
		return discard_chain;
	}

	void push_front_discard(wpl_value *value) {
		discard_chain.push_front(value);
	}

	void push_discard(wpl_value *value) {
		discard_chain.push(value);
	}

/*	void clear_discard() {
		discard_chain.clear();
	}*/

	int get_discard_pos() {
		return discard_chain.pos();
	}

	void set_discard_pos(int pos) {
		discard_chain.goto_pos(pos);
	}

	// RUNNING
	int run_runable_operator (
			wpl_runable_operator *runable,
			int index,
			wpl_value *lhs,
			wpl_value *rhs,
			wpl_value *final_result);
	int run_function (
		wpl_function *function,
		int index,
		int discard_pos,
		wpl_value *final_result,
		wpl_namespace_session *nss_this
	);

	int do_operator_on_unresolved (
		wpl_value_unresolved_identifier *unresolved,
		wpl_value *final_result
	) {
		return get_nss()->do_operator_on_unresolved (
			unresolved,
			this,
			final_result,
			WPL_NSS_CTX_SELF
		);
	}

	void set_loop_number(int loop_number) {
		flags |= CTX_LOOP;
		this->loop_number = loop_number;
	}

	int get_loop_number() {
		return loop_number;
	}

	int get_flags() {
		return flags;
	}
};
