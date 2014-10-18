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

#include "type.h"
#include "type_user_incomplete.h"
#include "namespace.h"
#include "type_parse_signals.h"

using namespace std;

void wpl_type_complete::parse_value (wpl_namespace *parent_namespace) {
	wpl_matcher_position begin_pos(get_position());

	ignore_whitespace();
	if (search_letter ('>')) {
		return;
	}

	char name[WPL_VARNAME_SIZE];
	try {
		get_word(name);
	}
	catch (wpl_element_exception &e) {
		cerr << "While parsing complete type '" << get_name() << "':\n";
		throw;
	}

	ignore_whitespace();
	if (ignore_letter ('(')) {
		throw wpl_type_begin_function_declaration(this, name, get_position());
	}
	else {
		throw wpl_type_begin_variable_declaration(this, name, begin_pos, get_position());
	}
}

void wpl_type_incomplete::parse_value (wpl_namespace *parent_namespace) {
	char buf[WPL_VARNAME_SIZE];
	try {
		get_word(buf);
	}
	catch (wpl_element_exception &e) {
		cerr << "While parsing '" << get_name() <<
		       "' definition:\n";
		throw;
	}

	wpl_type_user_incomplete *usr_obj = new_instance(buf);
	parent_namespace->add_managed_pointer (usr_obj);
	try {
		parent_namespace->register_parseable(usr_obj);
	}
	catch (wpl_exception_name_exists &e) {
		ostringstream msg;
		msg << "While registering name " <<
			e.get_name() <<
			" in namesapce: Name already defined\n";
		throw runtime_error(msg.str());
	}
	parent_namespace->add_type(usr_obj);

	usr_obj->load_position(get_position());
	usr_obj->parse_value(parent_namespace);

	throw runtime_error("Expected end statement exception when parsing incomplete type");
}

