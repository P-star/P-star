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

#include "value.h"

#include <memory>

class wpl_value_range : public wpl_value {
	private:
	wpl_value *notify;
	unique_ptr<wpl_value> counter;
	wpl_value *counter_target;
	int flags;
	const struct wpl_operator_struct *boolean_test;

	static const int INCLUSIVE = 1;
	static const int EXCLUSIVE = 2;
	static const int INCREMENT = 4;
	static const int DECREMENT = 8;
	static const int INITIALIZED = 16;

	public:
	wpl_value_range (wpl_value *notify) :
		notify(notify),
		flags(0),
		boolean_test(NULL)
	{}

	int get_precedence() const { return wpl_type_precedence_range; };
	const char *get_type_name() const { return wpl_typename_range; }

	wpl_value *clone() const {
		throw runtime_error("Cannot clone range value");
	};

	int do_range_operator (
		wpl_expression_state *exp_state,
		const struct wpl_operator_struct *op,
		wpl_value *lhs,
		wpl_value *rhs
	) override;

	private:
	bool counter_operate(wpl_expression_state *exp_state, const struct wpl_operator_struct *op);
};
