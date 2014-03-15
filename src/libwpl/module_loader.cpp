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

#ifndef WIN32

#include "module_loader.h"

#include <iostream>
#include <dlfcn.h>
#include <string>
#include <stdexcept>

using namespace std;

wpl_module_loader::wpl_module_loader (int argc, char **argv, const char *name) {
	string filename = string("libpstar-") + name + ".so";

	library_handle = dlopen (filename.c_str(), RTLD_LAZY);
	if (!library_handle) {
		cerr << "Loading of library '" << name <<
			"' failed (using filename '" << filename << "':\n";
		check_dlerror();
	}

	dlerror();

	*(void **) (&module_init) = dlsym(library_handle, "wpl_module_init");
	check_dlerror();
	*(void **) (&module_exit) = dlsym(library_handle, "wpl_module_exit");
	check_dlerror();
	*(void **) (&module_get_namespace) = dlsym(library_handle, "wpl_module_get_namespace");
	check_dlerror();

	module_init(argc, argv);
}

wpl_module_loader::~wpl_module_loader() {
	module_exit();
	dlclose(library_handle);
}

wpl_namespace *wpl_module_loader::get_namespace() {
	return module_get_namespace();
}

void wpl_module_loader::check_dlerror() {
	char *err;
	if (err = dlerror()) {
		cerr << err << endl;
		throw runtime_error("Library load failed");
	}
}

#endif /* WIN32 */
