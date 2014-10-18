/*

-------------------------------------------------------------

Copyright (c) MMXIV Atle Solbakken
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
#include "runable_operator.h"
#include "state.h"
#include "value.h"

class wpl_namespace_session;
class wpl_io;
struct wpl_operator_struct;

/**
 * @brief This runable is generated at run time inside expressions with the .. operator
 */
class wpl_range : public wpl_runable_operator {
	private:
	const struct wpl_operator_struct *boolean_test;
	wpl_expression_state dummy_exp_state;

	wpl_value *counter_begin;
	wpl_value *counter_end;

	int flags;
	static const int INCLUSIVE = 1;
	static const int EXCLUSIVE = 2;
	static const int INCREMENT = 4;
	static const int DECREMENT = 8;

	public:
	wpl_range (
		wpl_expression_state *exp_state,
		const wpl_operator_struct *op,
		wpl_value *lhs,
		wpl_value *rhs
	);

	wpl_state *new_state (wpl_state *parent, wpl_namespace_session *nss, wpl_io *io) override;

	int run (
		wpl_state *state,
		wpl_expression_state *exp_state,
		wpl_value *lhs,
		wpl_value *rhs,
		wpl_value *final_result
	) override;
};

class wpl_range_state : public wpl_state {
	friend class wpl_range;

	private:
	unique_ptr<wpl_value> counter;
	bool active;

	public:
	/*
	   Don't store RHS as it may be a dummy. Use new rhs each run.
	   */
	wpl_range_state (
		wpl_state *parent,
		wpl_namespace_session *nss,
		wpl_io *io,
		wpl_value *counter_begin,
		wpl_value *dummy
	) :
		wpl_state(parent, nss, io),
		active(false)
	{}

	protected:
	bool counter_operate(
			wpl_expression_state *exp_state,
			const struct wpl_operator_struct *op,
			wpl_value *counter_target
	);
	void reset(wpl_value *counter_begin) {
		counter.reset(counter_begin->clone());
	}
	bool is_active() {
		return active;
	}
	void set_active(bool value) {
		active = value;
	}
	wpl_value *clone_counter();
};
