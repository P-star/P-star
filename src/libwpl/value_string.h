/*

-------------------------------------------------------------

Copyright (c) MMXIII-MMXIV Atle Solbakken
atle@goliathdns.no
Copyright (c) MMXIV Sebastian Baginski
sebthestampede@gmail.com

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

#include "output_json.h"
#include "parasite.h"
#include "value_holder.h"

#include <cstdlib>
#include <string>

using namespace std;

template<typename A> class wpl_value_strings : public wpl_value_holder<A> {
	protected:

	int neq() {RESULT_LOGIC = (LHS != RHS); return WPL_OP_LOGIC_OK; };
	int eq() {RESULT_LOGIC = (LHS == RHS); return WPL_OP_LOGIC_OK; };
	int concat() {RESULT = LHS + RHS; return WPL_OP_OK; }
	int assign_concat() {RESULT = LHS + RHS; LHS = RESULT; return WPL_OP_OK|WPL_OP_DATA_MODIFIED|WPL_OP_ASSIGN; }
	int assign() {LHS = RHS; RESULT = RHS; return WPL_OP_OK|WPL_OP_DATA_MODIFIED|WPL_OP_ASSIGN; }
	int echo() {cout << RHS; return WPL_OP_OK; }
	int errcho() {cerr << RHS; return WPL_OP_OK; }
	int is_empty() {RESULT_LOGIC = (RHS).empty() ? true : false; return WPL_OP_LOGIC_OK; }

	void zero() {
		wpl_value_holder<A>::value = "";
	}

	virtual int __do_operator (const struct wpl_operator_struct *op) {
		CALL_OP(OP_CONCAT, concat)
		CALL_OP(OP_EQ, eq)
		CALL_OP(OP_NOT_EQ, neq)
		CALL_OP(OP_ASSIGN_CONCAT, assign_concat)
		CALL_OP(OP_ASSIGN, assign)
		CALL_OP(OP_ECHO, echo)
		CALL_OP(OP_ERRCHO, errcho)
		CALL_OP(OP_LOGIC_NOT, is_empty)
		CALL_OP(OP_IS_EMPTY, is_empty)
		return wpl_value_holder<A>::__do_operator(op);
	}
};

class wpl_value_string : public wpl_value_strings<string>, public wpl_parasite_host<wpl_value_string> {
	public:
	PRIMITIVE_CONSTRUCTOR(string,string,strings)
	PRIMITIVE_TYPEINFO(string)
	PRIMITIVE_SET_WEAK_NOTIFY(string,string,toString())
	PRIMITIVE_DO_OPERATOR_NOTIFY(string,toString())

/*	wpl_value_string (const char *new_value) : wpl_value_string() {
		value = new_value;
	}

	wpl_value_string(const char *new_value, int len)  : wpl_value_string() {
		value = string(new_value, len);
	}*/

	bool toBool() {
		return (!value.empty());
	}
	int toInt() {
		return strtol (value.c_str(), NULL, 0);
	}
	int long long toLongLongInt() {
		return strtoll (value.c_str(), NULL, 0);
	}
	unsigned int toUint() {
		return strtoul (value.c_str(), NULL, 0);
	}
	unsigned long long int toLongLongUint() {
		return strtoull (value.c_str(), NULL, 0);
	}
	float toFloat() {
		return strtof (value.c_str(), NULL);
	}
	double toDouble() {
		return strtod (value.c_str(), NULL);
	}
	string toString() const override {
		return value;
	}
	string &rawToString() {
		return value;
	}
	char* toVoid(){
		return (char*)value.c_str();
	}
	int get_size() {
		return value.size();
	}
	void set (int val) {
		value = val;
		notify_parasites();
	}
	void set (const char *val) {
		value = val;
		notify_parasites();
	}
	void set (string &new_string) {
		value = new_string;
		notify_parasites();
	}
	void operator= (const string &rhs) {
		value = rhs;
		notify_parasites();
	}

	virtual void output(wpl_io &io) override;
	void output_json(wpl_io &io) override;
};
