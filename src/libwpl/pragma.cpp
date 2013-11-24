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

#include "pragma.h"
#include "scene.h"
#include "exception.h"
#include "namespace.h"
#include "variable.h"
#include "value_unsafe_pointer.h"

#include <set>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <stdexcept>

using namespace std;

void wpl_pragma::parse_default_end() {
	ignore_string_match(WHITESPACE, 0);
	if (!ignore_letter(';')) {
		THROW_ELEMENT_EXCEPTION("Expected ';' after pragma");
	}
}

int wpl_pragma_template_var::run(wpl_state *state, wpl_value *final_result) {
	wpl_pragma_state *pragma_state = (wpl_pragma_state*) state;

	wpl_value_unsafe_pointer value_pointer;
	pragma_state->run_child (value_expression.get(), 0, &value_pointer);
	/*
	   TODO
	   To allow multiple values, replace unsafe_pointer with something that
	   collects all pointers from the discard chain, or create wpl_value_output_json_trigger
	 */
	/*
	   XXX
	   The expression subsystem no longer calls set_weak by default, this does not work
	 */
	set<wpl_value*> my_set;
	my_set.insert(value_pointer.dereference());

	my_template->output_json(
			(wpl_text_state*) pragma_state->get_child_state(my_template, 1),
			my_set,
			final_result
			);
	return WPL_OP_OK;
}

void wpl_pragma_fixed_text::parse_value(wpl_namespace *parent_namespace) {
	ignore_string_match(WHITESPACE, 0);
	if (!ignore_letter(';')) {
		cerr << "While parsing pragma '" << get_name() << "':\n";
		THROW_ELEMENT_EXCEPTION("Expected ';' after pragma");
	}
}

int wpl_pragma_fixed_text::run (wpl_state *state, wpl_value *final_result) {
	cout << text;
	return WPL_OP_OK;
}

void wpl_pragma_template_var::parse_value(wpl_namespace *parent_namespace) {
	char value[WPL_VARNAME_SIZE];

	// Find template name
	int len = search (WORD, WHITESPACE, false);
	if (len <= 0) {
		THROW_ELEMENT_EXCEPTION("Expected identifier name after pragma declaration");
	}
	check_varname_length(len);
	get_string (value, len);
	if (!(this->my_template = parent_namespace->find_template(value))){
		revert_string(len);
		THROW_ELEMENT_EXCEPTION("Unknown template identifier in pragma definition");
	}

	wpl_expression *exp = new wpl_expression();
	value_expression.reset(exp);

	exp->load_position(get_static_position());
	exp->parse_value(parent_namespace);
	load_position(exp->get_static_position());
}

void wpl_pragma_text::print() {
	fwrite (text_start, text_end-text_start, 1, stdout);
}

int wpl_pragma_text_content_type::run (wpl_state *state, wpl_value *final_result) {
	printf ("Content-type: ");
	print();
	printf (";\r\n\r\n");
	return WPL_OP_NO_RETURN;
}

void wpl_pragma_text::parse_value (wpl_namespace *parent_namespace) {
	ignore_string_match(WHITESPACE, 0);
	const char *start = get_string_pointer();

	if (!ignore_string_match(NON_SEMICOLON, 0)) {
		THROW_ELEMENT_EXCEPTION("Expected value after pragma definition");
	}

	const char *end = get_string_pointer();

	if (!ignore_letter (';')) {
		THROW_ELEMENT_EXCEPTION("Expected ';' after pragma definition");
	}

	set_text(start, end);
}

void wpl_pragma_add_all_to_namespace (wpl_namespace *my_namespace) {
	my_namespace->register_identifier((wpl_pragma *) new wpl_pragma_text_content_type());
	my_namespace->register_identifier((wpl_pragma *) new wpl_pragma_template());
	my_namespace->register_identifier((wpl_pragma *) new wpl_pragma_template_as_var());
	my_namespace->register_identifier((wpl_pragma *) new wpl_pragma_template_var());
	my_namespace->register_identifier((wpl_pragma *) new wpl_pragma_scene());
	my_namespace->register_identifier((wpl_pragma *) new wpl_pragma_json_begin());
	my_namespace->register_identifier((wpl_pragma *) new wpl_pragma_json_end());
}
