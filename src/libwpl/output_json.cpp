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

#include "output_json.h"
/*
   TODO
   implement as ostream
 */
void wpl_output_json::output_json(wpl_io &io, const char *str, int len) {
	string result;
	result.reserve(len*2);

	set_text(str, len);

	while (!at_end()) {
		const char *pos_start = get_string_pointer();
		int len = search (JSON_NEEDS_ESCAPE, NONCTRL_ASCII|UTF8, false);
		const char *pos_end = get_string_pointer();

		result.append(pos_start, pos_end-pos_start);

		if (len == 0) {
			break;
		}

		for (int i = 0; i < len; i++) {
			if (ignore_null_char()) {
				break;
			}
			char letter = get_letter();
			switch (letter) {
				case '"':
					result += '\\';
					result += '"';
					break;
				case '\\':
					result += '\\';
					result += '\\';
					break;
				case '\n':
					result += '\\';
					result += 'n';
					break;
				case '\r':
					result += '\\';
					result += 'r';
					break;
				case '\t':
					result += '\\';
					result += 't';
					break;
				default:
					result += letter;
			};
		}
	}

	if (!at_end()) {
		throw runtime_error("Could not convert string to JSON format");
	}

	io << result;
}
