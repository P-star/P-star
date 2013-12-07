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

#pragma once

#include "namespace.h"
#include "parser.h"
#include "debug.h"
#include "module_loader.h"

#include <iostream>
#include <list>

class wpl_io;

/* Module memory needs to be freed after namespace */
class wpl_program_base {
	protected:
	list<wpl_module_loader> modules;
};

/* Order of destruction of these two base classes is IMPORTANT */
class wpl_program : public wpl_program_base, public wpl_namespace {
	private:
	wpl_parser parser;

	int argc;
	char **argv;

	public:
	~wpl_program () {
#ifdef WPL_DEBUG_DESTRUCTION
		DBG("P: Destructing program\n");
#endif
	}

	wpl_program(int argc, char **argv);

	void parse_file (const char *filename);

	int run(wpl_io *io);
};
