/*

-------------------------------------------------------------

Copyright (c) MMXIII-MMXIX Atle Solbakken
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
#include "user_function.h"
#include "parser.h"
#include "type_parse_signals.h"

#include "global.h"

#include <memory>

extern const wpl_global_block *global_block;
wpl_struct::wpl_struct (const char *name, bool no_parsing) : wpl_type_user_incomplete(name) {
	parse_complete = no_parsing;
	parse_in_progress = false;
	destructor = NULL;
}

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
	char buf[WPL_VARNAME_SIZE+1];
	bool dtor_found = false;

	/* If we use or own type inside our struct, we should not set namespace 
	 * no ourself, which would otherwise create a loop in the namespace tree. */
	if (!parse_in_progress) {
		set_parent_namespace(ns);
	}

	wpl_matcher_position begin_pos(get_position());
	ignore_string_match(WHITESPACE,0);

	if (parse_complete || parse_in_progress) {
		ignore_whitespace();
		if (search_letter ('>')) {
			return;
		}

		// Check for constructor disabler. When the definition
		// laster is parsed again as an expression, this * is used,
		// but not here.
		if (ignore_letter ('*')) {
			ignore_whitespace();
		}

		// Check for variable name
		try {
			get_word(buf);
		}
		catch (wpl_element_exception &e) {
			cerr << "After struct name '" << get_name() << "':\n";
			throw;
		}

		throw wpl_type_begin_variable_declaration(this, buf, begin_pos, get_position());
	}

	parse_in_progress = true;

	/*
	   TODO allow declaration only now and definition later
	 */
	if (!ignore_letter('{')) {
		THROW_ELEMENT_EXCEPTION("Expected block with declarations after struct declaration");
	}

	ignore_whitespace();

	if (ignore_letter ('}')) {
		goto no_members;
	}

	do {
		ignore_whitespace();

		wpl_matcher_position def_begin = get_position();

		// Check for comment
		if (ignore_string ("/*")) {
			wpl_parser::parse_comment(this);
			ignore_whitespace();
		}

		// Check for destructor
		if (ignore_letter('~')) {
			if (dtor_found) {
				load_position(def_begin);
				THROW_ELEMENT_EXCEPTION("Can't have more than one destructor definition (~)");
			}

			get_word(buf);
			if (!is_name(buf)) {
				load_position(def_begin);
				ostringstream tmp;
				tmp << "Destructor definition '~" << buf << "' did not match struct name '" << get_name() << "'";
				THROW_ELEMENT_EXCEPTION(tmp.str());
			}

			ignore_whitespace();
			if (!ignore_letter('(')) {
				THROW_ELEMENT_EXCEPTION("Expected ( after destructor definition");
			}

			string dtor_name("~");
			dtor_name += buf;

			wpl_user_function *function = new wpl_user_function(wpl_type_global_void, dtor_name.c_str(), WPL_VARIABLE_ACCESS_PRIVATE);
			register_identifier(function);
			destructor = function;

			function->load_position(get_position());
			function->parse_value(this);
			load_position(function->get_position());

			if (function->variables_count() > 0) {
				load_position(def_begin);
				THROW_ELEMENT_EXCEPTION("Destructor function was defined with arguments");
			}

			dtor_found = true;

			goto definition_out;
		}

		get_word(buf);

		// Check for constructor
		if (is_name(buf)) {
			ignore_whitespace();
			if (!ignore_letter ('(')) {
				load_position(def_begin);
				THROW_ELEMENT_EXCEPTION("Expected ( after constructor definition");
			}

			wpl_user_function *function = new wpl_user_function(wpl_type_global_void, buf, WPL_VARIABLE_ACCESS_PRIVATE);
			register_hidden_identifier(function);

			function->load_position(get_position());
			function->parse_value(this);
			load_position(function->get_position());

			goto definition_out;
		}

		// Check for other parseables
		{
			wpl_parseable_identifier *parseable;
			if (!(parseable = global_block->find_parseable(buf))) {
				if (!(parseable = find_parseable(buf))) {
					load_position(def_begin);
					cerr << "While parsing name '" << buf << "' inside struct:\n";
					THROW_ELEMENT_EXCEPTION("Undefined name");
				}
			}

			parseable->load_position(get_position());
			load_position(wpl_parser::parse_parseable_identifier(this, this, parseable));

			/*
			try {
				try {
					parseable->parse_value(this);
				}
				catch (wpl_type_begin_variable_declaration &e) {
					e.create_variable(this);
					load_position(parseable->get_position());
				}
			}
			catch (wpl_type_begin_function_declaration &e) {
				e.parse_value(this);
				load_position(e.get_position());
			}
	*/
			goto definition_out;
		}

		definition_out:

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
	parse_in_progress = false;
	throw wpl_type_end_statement(get_position());
}
