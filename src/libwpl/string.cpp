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

using namespace std;

#define WPL_PROCESS_ESCAPE_CHAR(escape,replace) \
	if (letter == escape) { *target=replace; return true; }

bool wpl_string_parse_double_escape(char *target, char letter) {
	WPL_PROCESS_ESCAPE_CHAR('/', '/')
	WPL_PROCESS_ESCAPE_CHAR('\0', 0)
	WPL_PROCESS_ESCAPE_CHAR('a', 7)
	WPL_PROCESS_ESCAPE_CHAR('b', 8)
	WPL_PROCESS_ESCAPE_CHAR('t', 9)
	WPL_PROCESS_ESCAPE_CHAR('n', 10)
	WPL_PROCESS_ESCAPE_CHAR('v', 11)
	WPL_PROCESS_ESCAPE_CHAR('r', 13)
	WPL_PROCESS_ESCAPE_CHAR('"', 34)
	WPL_PROCESS_ESCAPE_CHAR('\\', 92)
	WPL_PROCESS_ESCAPE_CHAR('$', 36)
	WPL_PROCESS_ESCAPE_CHAR('{', 123)
	WPL_PROCESS_ESCAPE_CHAR('}', 124)

	return false;
}

bool wpl_string_parse_single_escape(char *target, char letter) {
	WPL_PROCESS_ESCAPE_CHAR('\'', 39)
	return false;
}

