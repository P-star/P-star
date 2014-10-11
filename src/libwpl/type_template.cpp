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
#include "type_parse_signals.h"

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

	wpl_type_complete *complete_type;
	wpl_type_template *template_type;

	if (complete_type = parent_namespace->find_complete_type(buf)) {
		/* Parse a complete type like 'int'. We do not allow statements
		   like array<int a>, throw exception if this happens. */
		complete_type->load_position(get_position());

		try {
			complete_type->parse_value(parent_namespace);
		}
		catch (wpl_type_begin_declaration &e) {
			THROW_ELEMENT_EXCEPTION("Variable declaration not allowed inside template definition. Maybe you forgot a '>'?");
		}
	}
	else if (template_type = parent_namespace->find_template_type(buf)) {
		/* Parse another template type, like if we have array<array<int>> */
		template_type->load_position(get_position());

		try {
			template_type->parse_value(parent_namespace);
		}
		catch (wpl_type_begin_declaration &e) {
			THROW_ELEMENT_EXCEPTION("Variable declaration not allowed inside template definition. Maybe you forgot a '>'?");
		}

		/* The other template type will create a new complete type
		   in the namespace which we retrieve here */
		complete_type = parent_namespace->get_last_complete_type();
	}
	else {
		cerr << "While parsing name '" << buf << "' inside template definition:\n";
		THROW_ELEMENT_EXCEPTION("Undefined name");
	}

	load_position(complete_type->get_position());
	ignore_whitespace();
	if (!ignore_letter('>')) {
		cerr << "While parsing complete type " << complete_type->get_name() << " derived from template:";
		THROW_ELEMENT_EXCEPTION("Expected > after template definition");
	}

	/* Create a new complete type, like array<int>. */
	complete_type = register_unique_complete_type(parent_namespace, complete_type);
	complete_type->load_position(get_position());

	/* Use this since we've got not finally{} in C++, the variable
	   declaration is thrown inside parse_value below */
	wpl_smart_load_position lp(this, complete_type);

	complete_type->parse_value(parent_namespace);
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
	}

	/* We need to do this even if were already added so that our
	   caller can find the correct value */
	parent_namespace->add_type(complete_type);

	return complete_type;
}
