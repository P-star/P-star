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

#include "type_template.h"

wpl_type_complete_template::wpl_type_complete_template (
		const wpl_type_template *mother_type,
		const wpl_type_complete *template_type
		) :
	wpl_type_complete((string(mother_type->get_name()) + "<" + template_type->get_name() + ">").c_str())
{
	this->template_type = template_type;
	this->mother_type = mother_type;
}

void wpl_type_template::parse_value (wpl_namespace *parent_namespace) {
	char buf[WPL_VARNAME_SIZE];

	ignore_whitespace();
	if (!ignore_letter ('<')) {
		THROW_ELEMENT_EXCEPTION("Expected '<' after template type name");
	}

	try {
		get_word(buf);
	}
	catch (wpl_element_exception &e) {
		cerr << "While parsing name for template sub-type:\n";
		throw;
	}

	wpl_parseable *parseable;
	if (!(parseable = parent_namespace->new_find_parseable(buf))) {
		cerr << "While parsing name '" << buf << "' inside template definition:\n";
		THROW_ELEMENT_EXCEPTION("Undefined name");
	}

	try {
		parseable->load_position(get_position());
		parseable->parse_value(parent_namespace);
	}
	catch (wpl_type_end_template_declaration &e) {
		load_position(parseable->get_position());
		parseable = register_unique_complete_type(parent_namespace, e.get_type());

		try {
			parseable->load_position(get_position());
			parseable->parse_value(parent_namespace);
		}
		catch (wpl_type_end_template_declaration &e) {
			load_position(parseable->get_position());
			parseable = register_unique_complete_type(parent_namespace, e.get_type());
			parseable->load_position(get_position());
			parseable->parse_value(parent_namespace);
		}
	}
}

wpl_type_complete *wpl_type_template::register_unique_complete_type (
		wpl_namespace *parent_namespace,
		const wpl_type_complete *type
) {
	unique_ptr<wpl_type_complete> new_type(new_instance(type));

	// Check if this template type is already defined
	wpl_type_complete *complete_type = NULL;
	if (!(complete_type = parent_namespace->find_complete_type(new_type->get_name()))) {
		parent_namespace->new_register_parseable(new_type.get());
		complete_type = new_type.release();
		parent_namespace->add_managed_pointer(complete_type);
		parent_namespace->add_complete_type(complete_type);
	}

	return complete_type;
}
