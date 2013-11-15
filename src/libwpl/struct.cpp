/*

-------------------------------------------------------------

Copyright (c) MMIII Atle Solbakken
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
#ifdef WPL_DEBUG_DESTRUCTIOM
	DBG("ST: Destructing struct");
#endif
}

wpl_value *wpl_struct::new_instance() const {
	wpl_value_struct *ret = new wpl_value_struct(this);
	return ret;
}

void wpl_struct::parse_value(wpl_namespace *ns) {
	ignore_string_match(WHITESPACE,0);
	/*
	   TODO allow declaration only now and definition later
	 */
	if (!ignore_letter('{')) {
		THROW_ELEMENT_EXCEPTION("Expected block with declarations after struct declaration");
	}

	wpl_block block;

	block.load_position(get_static_position());
	block.parse_value_only_declarations(ns);
	load_position(block.get_static_position());

	if (!ignore_letter(';')) {
		THROW_ELEMENT_EXCEPTION("Expected ';' after struct declaration");
	}
}
