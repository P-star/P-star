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

#include "modifier.h"
#include "namespace.h"
#include "global.h"

extern const wpl_global_block *global_block;

void wpl_modifier::parse_value (wpl_namespace *parent_namespace) {
	char buf[WPL_VARNAME_SIZE];
	get_word(buf);

	wpl_parseable *parseable;
	if (!(parseable = global_block->find_parseable (buf))) {
		if (!(parseable = parent_namespace->find_parseable (buf))) {
			cerr << "While parsing name '" << buf << 
				"' after modifier '" << get_name() << "':\n";
			THROW_ELEMENT_EXCEPTION("Undefined name");
		}
	}

	parseable->load_position(get_position());
	try {
		parseable->parse_value(parent_namespace);
	}
	catch (wpl_type_begin_declaration &e) {
		modify_access_flags(e);
		load_position(parseable->get_position());
		throw;
	}
}

void wpl_modifier_add_all_to_namespace(wpl_namespace *parent_namespace) {
	static wpl_modifier_protected	constant_protected;
	static wpl_modifier_private	constant_private;
	static wpl_modifier_public	constant_public;

	parent_namespace->register_parseable (&constant_protected);
	parent_namespace->register_parseable (&constant_private);
	parent_namespace->register_parseable (&constant_public);
}
