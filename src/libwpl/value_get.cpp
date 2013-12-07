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

#include "types.h"
#include "value_get.h"
#include "operator.h"
#include "value_string.h"
#include "value_array.h"
#include "matcher.h"

#include <memory>
#include <cstring>
#include <map>

extern wpl_type_array *wpl_type_global_array;

wpl_type_array_instance wpl_value_get::type_complete_array(
		wpl_type_global_array, wpl_type_global_bool
);

void wpl_value_get::parse(const char *query_string) {
	int len = strlen(query_string);

	if (len <= 0) {
		return;
	}

	wpl_matcher matcher;
	matcher.set_text(query_string, len);

	bool do_value = false;

	string buf;
	string buf_key;
	buf.reserve(len);
	buf_key.reserve(len);

	bool finish = false;

	while (true) {
		char letter = '\0';
		if (!matcher.at_end()) {
			letter = matcher.get_letter();
		}
		else {
			finish = true;
		}

//		cout << "Parse letter " << letter << "\n";
		if (M_AMP(letter) || M_SEMICOLON(letter) || finish) {
			wpl_value *new_val;

			if (matcher.at_end() && letter) {
				buf.push_back(letter);
			}

			if (do_value) {
				new_val = new wpl_value_string(buf);
			}
			else {
				new_val = new wpl_value_bool(true);
				buf_key = buf;
			}

			wpl_value_array *storage = (wpl_value_array*) hash.get(buf_key);
			if (!storage) {
				storage = (wpl_value_array*) hash.define(buf_key);
			}
			storage->push(new_val);

			if (matcher.at_end()) {
				break;
			}

			do_value = false;
			buf_key = "";
			buf = "";
		}
		else if (M_QUERY_NOT_ENCODED(letter)) {
			buf.push_back(letter);
		}
		else if (M_PERCENT(letter)) {
			if (matcher.get_remaining_length() < 2) {
				throw runtime_error("Invalid query string: Not enough characters after '%'");
			}

			char tmp[3];
			matcher.get_string(tmp, 2);

			if (!M_HEX(tmp[0]) || !M_HEX(tmp[1])) {
				throw runtime_error("Invalid query string: Invalid characters in '%'-sequence");
			}

			int num = strtol(tmp, NULL, 16);
			if (num == 0 || M_INVALID(num)) {
				cerr << "While parasing character 0x" << hex << num << "\n";
				throw runtime_error("Invalid character in %-sequence in query string");
			}
			buf.push_back(num);
		}
		else if (M_ADD(letter)) {
			buf.push_back(' ');
		}
		else if (M_EQ(letter)) {
			if (do_value) {
				throw runtime_error("Invalid query string: Malplaced '='");
			}
			buf_key = buf;
			buf = "";
			do_value = true;
		}
	}

	did_parse = true;
}

int wpl_value_get::do_operator (
		wpl_expression_state *exp_state,
		wpl_value *final_result,
		const wpl_operator_struct *op,
		wpl_value *lhs,
		wpl_value *rhs
) {
	if (!did_parse) {
		const char *query_string = exp_state->get_io().getenv("QUERY_STRING");
		if (!query_string) {
			throw runtime_error("Could not find the QUERY_STRING environment variable while constructing GET-object");
		}

		parse(query_string);
	}
	return hash.do_operator(
			exp_state,
			final_result,
			op,
			(lhs == this ? &hash : lhs),
			(rhs == this ? &hash : rhs)
			);
}
