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

#include "value_holder.h"
#include "operator.h"
#include "value.h"

#include <string>

using namespace std;

// We only check the pointer, assuming people don't
// create their own operator definition structs
#define CALL_OP(_op, func) \
	if (op == &_op) { return func(); }

template<typename A> class wpl_value_number : public wpl_value_holder<A> {
	private:

	int add() {RESULT = LHS + RHS; return WPL_OP_OK; }
	int sub() {RESULT = LHS - RHS; return WPL_OP_OK; }
	int mul() {RESULT = LHS * RHS; return WPL_OP_OK; }
	int div() {
		if (RHS == 0) {
			throw runtime_error("Divide by zero prevented");
		}
		RESULT = LHS / RHS;
		return WPL_OP_OK;
	}

	int assign_sum() {RESULT = LHS += RHS; return WPL_OP_OK|WPL_OP_ASSIGN; }
	int assign_sub() {RESULT = LHS -= RHS; return WPL_OP_OK|WPL_OP_ASSIGN; }
	int assign_mul() {RESULT = LHS *= RHS; return WPL_OP_OK|WPL_OP_ASSIGN; }
	int assign_div() {RESULT = LHS /= RHS; return WPL_OP_OK|WPL_OP_ASSIGN; }

	int assign() {LHS = RHS; RESULT = RHS; return WPL_OP_OK|WPL_OP_ASSIGN; }

	int echo() {cout << RHS; return WPL_OP_OK; }
	int errcho() {cerr << RHS; return WPL_OP_OK; }

	int logic_or() {RESULT_LOGIC = (LHS || RHS ? true : false); return WPL_OP_LOGIC_OK; }
	int logic_and() {RESULT_LOGIC = (LHS && RHS ? true : false); return WPL_OP_LOGIC_OK; }
	int logic_not() {RESULT_LOGIC = (RHS ? false : true); return WPL_OP_LOGIC_OK; }

	int eq() {RESULT_LOGIC = (LHS==RHS ? true : false); return WPL_OP_LOGIC_OK; }
	int neq() {RESULT_LOGIC = (LHS!=RHS ? true : false); return WPL_OP_LOGIC_OK; }
	int lt() {RESULT_LOGIC = (LHS<RHS ? true : false); return WPL_OP_LOGIC_OK; }
	int gt() {RESULT_LOGIC = (LHS>RHS ? true : false); return WPL_OP_LOGIC_OK; }
	int lteq() {RESULT_LOGIC = (LHS<=RHS ? true : false); return WPL_OP_LOGIC_OK; }
	int gteq() {RESULT_LOGIC = (LHS>=RHS ? true : false); return WPL_OP_LOGIC_OK; }

	protected:
	virtual int __do_operator (const struct wpl_operator_struct *op) {
		int res;
		if ((res = wpl_value_holder<A>::__do_operator(op)) != WPL_OP_UNKNOWN) {
			return res;
		}

		CALL_OP(OP_LT, lt)
		CALL_OP(OP_GT, gt)
		CALL_OP(OP_LTEQ, lteq)
		CALL_OP(OP_GTEQ, gteq)
		CALL_OP(OP_SUM, add)
		CALL_OP(OP_SUB,sub)
		CALL_OP(OP_MUL,mul)
		CALL_OP(OP_DIV,div)
		CALL_OP(OP_ASSIGN_SUM,assign_sum)
		CALL_OP(OP_ASSIGN_SUB,assign_sub)
		CALL_OP(OP_ASSIGN_MUL,assign_mul)
		CALL_OP(OP_ASSIGN_DIV,assign_div)
		CALL_OP(OP_ASSIGN, assign)
		CALL_OP(OP_ECHO, echo)
		CALL_OP(OP_ERRCHO, errcho)
		CALL_OP(OP_LOGIC_OR, logic_or)
		CALL_OP(OP_LOGIC_AND, logic_and)
		CALL_OP(OP_LOGIC_NOT, logic_not)
		CALL_OP(OP_EQ, eq)
		CALL_OP(OP_NOT_EQ, neq)
		return WPL_OP_UNKNOWN;
	}

	public:

	virtual void output(wpl_io &io) override {
		io << wpl_value_holder<A>::value;
	}

	virtual void output_json(wpl_io &io) override {
		io << wpl_value_holder<A>::value;
	}

	void zero() {
		wpl_value_holder<A>::value = 0;
	}
	char *toVoid() {
		return (char*) &(wpl_value_holder<A>::value);
	}
	bool toBool() override {
		return (wpl_value_holder<A>::value != 0);
	}
	int toInt() override {
		return (int) wpl_value_holder<A>::value;
	}
	long long int toLongLongInt() override {
		return (long long int) wpl_value_holder<A>::value;
	}
	unsigned int toUint() override {
		return (unsigned int) wpl_value_holder<A>::value;
	}
	unsigned long long int toLongLongUint() override {
		return (unsigned long long int) wpl_value_holder<A>::value;
	}
	float toFloat() override {
		return (float) wpl_value_holder<A>::value;
	}
	double toDouble() override {
		return (double) wpl_value_holder<A>::value;
	}
	string toString() const override {
		return to_string(wpl_value_holder<A>::value);
	}
};


// Put this at the bottom :-D
#include "value_bool.h"
