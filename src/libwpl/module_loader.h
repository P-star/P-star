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

#ifndef WIN32
#include "module.h"

using namespace std;

class wpl_module_loader {
	private:
	void *library_handle;

	int (*module_init)(int argc, char **argv);
	void (*module_exit)();
	wpl_namespace *(*module_get_namespace)();

	void check_dlerror();

	public:
	wpl_module_loader (int argc, char **argv, const char *name);
	~wpl_module_loader();
	wpl_namespace *get_namespace();
};
#endif
