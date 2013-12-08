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

#include "struct.h"
#include "exception.h"
#include "variable.h"
#include "block.h"
#include "value.h"
#include "value_struct.h"
#include "debug.h"

#include <memory>

wpl_struct::~wpl_struct() {
#ifdef WPL_DEBUG_DESTRUCTION
	DBG("ST: Destructing struct");
#endif
}

wpl_value *wpl_struct::new_instance() const {
	wpl_value_struct *ret = new wpl_value_struct(this);
	return ret;
}

void wpl_struct::parse_value(wpl_namespace *ns) {
	char buf[WPL_VARNAME_SIZE];
	wpl_matcher_position begin_pos(*get_static_position());
	ignore_string_match(WHITESPACE,0);

	if (parse_complete) {
		ignore_whitespace();
		if (ignore_letter ('>')) {
			throw wpl_type_end_template_declaration(this);
		}
		try {
			get_word(buf);
		}
		catch (wpl_element_exception &e) {
			cerr << "After struct name '" << get_name() << "':\n";
			throw;
		}
		throw wpl_type_begin_variable_declaration(this, buf, begin_pos);
	}

	/*
	   TODO allow declaration only now and definition later
	 */
	if (!ignore_letter('{')) {
		THROW_ELEMENT_EXCEPTION("Expected block with declarations after struct declaration");
	}

	if (ignore_letter ('}')) {
		goto no_members;
	}

	do {

		get_word(buf);

		wpl_parseable *parseable;
		if (!(parseable = ns->new_find_parseable(buf))) {
			load_position(&begin_pos);
			cerr << "While parsing name '" << buf << "' inside struct:\n";
			THROW_ELEMENT_EXCEPTION("Undefined name");
		}

		parseable->load_position(get_static_position());

		try {
			try {
				parseable->parse_value(this);
			}
			catch (wpl_type_begin_variable_declaration &e) {
				e.create_variable(this);
				load_position(parseable->get_static_position());
			}
		}
		catch (wpl_type_begin_function_declaration &e) {
			e.parse_value(this);
			load_position(e.get_static_position());
		}

		ignore_whitespace();
		if (!ignore_letter (';')) {
			THROW_ELEMENT_EXCEPTION("Expected ';' after definition in struct");
		}
		ignore_whitespace();

		if (ignore_letter ('}')) {
			break;
		}
	} while (!at_end());

	no_members:

	parse_complete = true;
	throw wpl_type_end_statement(get_static_position());
}
