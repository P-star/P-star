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

#include "type_parse_signals.h"
#include "user_function.h"
#include "variable.h"
#include "namespace.h"

wpl_type_begin_function_declaration::wpl_type_begin_function_declaration(
		wpl_type_complete *type,
		const char *name,
		const wpl_matcher_position &pos
) :
	wpl_type_begin_declaration (type, name, WPL_VARIABLE_ACCESS_PRIVATE),
	wpl_matcher(pos)
{}

void wpl_type_begin_function_declaration::parse_value (wpl_namespace *parent_namespace) {
	wpl_user_function *function = new wpl_user_function(type, name.c_str(), access_flags);
	parent_namespace->register_identifier(function);

	function->load_position(get_position());
	function->parse_value(parent_namespace);
	load_position(function->get_position());
}

wpl_type_begin_variable_declaration::wpl_type_begin_variable_declaration (
	wpl_type_complete *type,
	const char *name,
	const wpl_matcher_position &position_at_name,
	const wpl_matcher_position &position_after_name
) :
	position_at_name(position_at_name),
	wpl_matcher(position_after_name),
	wpl_type_begin_declaration (type, name, WPL_VARIABLE_ACCESS_PRIVATE)
{}

void wpl_type_begin_variable_declaration::create_variable (wpl_namespace *parent_namespace) {
	wpl_variable_holder new_var (name.c_str(), type->new_instance(), access_flags);
	parent_namespace->register_identifier(&new_var);
}
