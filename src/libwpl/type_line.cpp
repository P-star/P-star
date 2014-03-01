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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with P*. If not, see <http://www.gnu.org/licenses/>.

-------------------------------------------------------------

*/

#include "type_line.h"

#include "types.h"
#include "exception.h"
#include "function.h"

wpl_type_line::wpl_type_line() :
	wpl_struct(wpl_typename_line, true)
{
//	wpl_variable_holder this_var("this", new wpl_value_MYSQL_STMT(), WPL_VARIABLE_ACCESS_PRIVATE);
//	register_identifier(&this_var);

//	register_identifier(new wpl_file_open());
}

wpl_type_line constant_type_line;

void wpl_type_line_register (wpl_namespace *target) {
	target->new_register_parseable(&constant_type_line);
}
