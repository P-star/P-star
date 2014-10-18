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

#include "template.h"
#include "expression.h"
#include "exception.h"

/* From text.cpp */
void wpl_text_add_parse_and_run_to_ns(wpl_namespace *ns);

/* From pragma.cpp */
void wpl_pragma_add_template_stuff_to_namespace(wpl_namespace *ns);

void wpl_template_add_parse_and_run_to_ns(wpl_namespace *ns) {
	wpl_text_add_parse_and_run_to_ns(ns);
	wpl_pragma_add_template_stuff_to_namespace(ns);
}
