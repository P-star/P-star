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

#pragma once

enum {
	wpl_type_precedence_lowest,

	wpl_type_precedence_unsafe_pointer,
	wpl_type_precedence_constant_pointer,
	wpl_type_precedence_double_finalizer,
	wpl_type_precedence_wrapper,
	wpl_type_precedence_range,
	wpl_type_precedence_output_trigger,
	wpl_type_precedence_unresolved_identifier,
	wpl_type_precedence_discard_chain,
	wpl_type_precedence_expression,
	wpl_type_precedence_list,
	wpl_type_precedence_void,
	wpl_type_precedence_vstring,
	wpl_type_precedence_bool,
	wpl_type_precedence_int,
	wpl_type_precedence_llint,
	wpl_type_precedence_uint,
	wpl_type_precedence_lluint,
	wpl_type_precedence_float,
	wpl_type_precedence_double,
	wpl_type_precedence_string,
	wpl_type_precedence_struct,
	wpl_type_precedence_class,
	wpl_type_precedence_array,
	wpl_type_precedence_pointer,
	wpl_type_precedence_hash,
	wpl_type_precedence_regex,

	wpl_type_precedence_env,
	wpl_type_precedence_get,
	wpl_type_precedence_post,
	wpl_type_precedence_stdin,
	wpl_type_precedence_time,
	wpl_type_precedence_file,
	wpl_type_precedence_line,
	wpl_type_precedence_auto,

	wpl_type_precedence_middle,
	
	wpl_type_precedence_function_ptr,

	wpl_type_precedence_highest
};
