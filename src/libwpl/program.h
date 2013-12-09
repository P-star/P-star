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

#include "io.h"
#include "namespace.h"
#include "parser.h"
#include "debug.h"
#include "module_loader.h"

#include <mutex>
#include <iostream>
#include <list>
	

class wpl_program : public wpl_namespace {
	private:
	wpl_parser parser;

	wpl_io *io;
	int argc;
	char **argv;

	public:
	~wpl_program () {
#ifdef WPL_DEBUG_DESTRUCTION
		DBG("P: Destructing program\n");
#endif
	}

	wpl_program(wpl_io &io, int argc, char **argv);

	void parse_file (const char *filename);

	int run();
};
