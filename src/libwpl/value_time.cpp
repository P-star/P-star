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

#include "value_time.h"
#include "operator.h"
#include "value_int.h"
#include "value_string.h"

#include <ctime>
#include <cstring>
#include <sstream>

void wpl_value_time::set_weak (wpl_value *value) {
	/*
	   TODO
	   Support setting from string and int
	   */
	wpl_value_time *value_time = dynamic_cast<wpl_value_time*>(value);
	if (value_time == NULL) {
		ostringstream tmp;
		tmp << "Could not set TIME object to value of type " << value->get_type_name();
		throw runtime_error(tmp.str());
	}
	else {
		set_time(value_time->get_time());
		set_format(value_time->get_format());
	}
}

string wpl_value_time::toString() {
	string tmp;
	format_time(NULL, tmp);
	return tmp;
}

int wpl_value_time::do_operator (
		wpl_expression_state *exp_state,
		wpl_value *final_result,
		const wpl_operator_struct *op,
		wpl_value *lhs,
		wpl_value *rhs
) {
	if (op == &OP_ELEMENT) {
		const char *cmd = rhs->toString().c_str();

		/*
		   Commands which return self
		 */
		if (strcmp (cmd, "set_now") == 0) {
			set_now();
			return do_operator_recursive(exp_state, final_result);
		}

		/*
		   Commands which return strings
		 */
		{
			wpl_value_string result("");
			bool ok = true;

			if (strcmp (cmd, "rfc2822") == 0) {
				format_time("%a, %d %b %Y %T %z", result.get_internal_string());
			}
			else if (strcmp (cmd, "iso8601") == 0) {
				format_time("%Y-%m-%dT%H:%M:%S%z", result.get_internal_string());
			}
			else {
				ok = false;
			}

			if (ok) {
				return result.do_operator_recursive(exp_state, final_result);
			}
		}

		/*
		   Commands which return integers
		 */
		{
			time_t t = get_time();
			struct tm time_tm;
			localtime_r(&t, &time_tm);

			wpl_value_int result(0);
			bool ok = true;

			if (strcmp (cmd, "year") == 0) {
				result.set(time_tm.tm_year + 1900);
			}
			else if (
				(strcmp (cmd, "month") == 0) ||
				(strcmp (cmd, "mon") == 0)
			) {
				result.set(time_tm.tm_mon + 1);
			}
			else if (strcmp (cmd, "day") == 0) {
				result.set(time_tm.tm_mday);
			}
			else if (strcmp (cmd, "hour") == 0) {
				result.set(time_tm.tm_hour);
			}
			else if (strcmp (cmd, "min") == 0) {
				result.set(time_tm.tm_min);
			}
			else if (strcmp (cmd, "sec") == 0) {
				result.set(time_tm.tm_sec);
			}
			else {
				ok = false;
			}

			if (ok) {
				return result.do_operator_recursive(exp_state, final_result);
			}
		}

		ostringstream tmp;
		tmp << "Member '" << cmd << "' is uknown for TIME objects";
		throw runtime_error(tmp.str());
	}
	else if (op == &OP_CONCAT) {
		wpl_value_string result("");
		string &tmp = result.get_internal_string();

		format_time(NULL, tmp);
		tmp += rhs->toString();

		return result.do_operator_recursive(exp_state, final_result);
	}
	else if (op == &OP_ARRAY_SUBSCRIPTING) {
		wpl_value_string result("");
	
		format_time(rhs->toString().c_str(), result.get_internal_string());
	
		return result.do_operator_recursive(exp_state, final_result);
	}
	else if (op == &OP_ASSIGN) {
		set_weak(rhs);
		return do_operator_recursive(exp_state, final_result);
	}
	else if (op == &OP_FUNCTION_CALL) {
		throw runtime_error("Unexpected function call () after TIME object");
	}

	return WPL_OP_UNKNOWN;
}
