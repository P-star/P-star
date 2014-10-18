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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with P*.  If not, see <http://www.gnu.org/licenses/>.

-------------------------------------------------------------

*/

#pragma once

#include "namespace.h"

#include <memory.h>

class wpl_global : public wpl_namespace {
};

class wpl_global_toplevel : public wpl_global {
	public:
	wpl_global_toplevel() : wpl_global() {
		wpl_namespace::set_toplevel();
	}
};

/* From block.cpp */
void wpl_block_add_parse_and_run_to_ns(wpl_namespace *ns);

/* From text.cpp */
void wpl_text_add_parse_and_run_to_ns(wpl_namespace *ns);

/* From template.cpp */
void wpl_template_add_parse_and_run_to_ns(wpl_namespace *ns);

class wpl_global_block : public wpl_global {
	public:
	wpl_global_block() :
		wpl_global()
	{
		wpl_block_add_parse_and_run_to_ns(this);
	}
};

class wpl_global_text : public wpl_global {
	public:
	wpl_global_text() :
		wpl_global()
	{
		wpl_text_add_parse_and_run_to_ns(this);
	}
};

class wpl_global_template : public wpl_global {
	public:
	wpl_global_template() :
		wpl_global()
	{
		wpl_template_add_parse_and_run_to_ns(this);
	}
};
