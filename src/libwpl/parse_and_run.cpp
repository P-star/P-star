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

#include "parse_and_run.h"

#include "type_parse_signals.h"

/**
 * @brief Parse a parseable with unknown, but funky syntax. The parseable can add something to our namespace if it wants to.
 */
void wpl_parse_and_run::parse_parseable_identifier(wpl_namespace *ns, wpl_parseable_identifier *parseable) {
	parseable->load_position(get_position());

	try {
		try {
			parseable->parse_value(ns);
		}
		catch (wpl_type_begin_variable_declaration &e) {
			e.create_variable(ns);
			load_position(e.get_position_at_name());
		}
	}
	catch (wpl_type_begin_function_declaration &e) {
		e.load_position(parseable->get_position());
		e.parse_value(ns);
		load_position(e.get_position());
	}
}
