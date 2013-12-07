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

#include "program.h"
#include "parser.h"
#include "pragma.h"
#include "types.h"
#include "value.h"
#include "value_array.h"
#include "value_string.h"
#include "value_int.h"
#include "namespace_session.h"
#include "io.h"

#include <utility>
#include <memory>

wpl_program::wpl_program(wpl_io &io, int argc, char **argv) : parser(io, 0) {
	this->io = &io;

	wpl_types_add_all_to_namespace(this);
	wpl_pragma_add_all_to_namespace(this);

	unique_ptr<wpl_value_array> val_argv (
			new wpl_value_array (
				wpl_type_global_string,
				argc
				)
			);

	for (int i = 0; i < argc; i++) {
		val_argv->set (i, new wpl_value_string(argv[i]));
	}

	wpl_variable_holder new_variable ("argv", val_argv.release(), WPL_VARIABLE_ACCESS_PRIVATE);
	new_variable.setStatic();

	register_identifier(&new_variable);

	list<wpl_module_loader>::iterator it;

#ifndef WIN32
	it = modules.emplace(modules.end(), argc, argv, "mysql");
	insert_parent_namespace(it->get_namespace());
#endif
}

void wpl_program::parse_file (const char *filename) {
	parser.parse_file(this, filename);
}

int wpl_program::run() {
	int ret;
	wpl_value *value;
	wpl_value_return retval;
	wpl_namespace_session nss;
	copy_variables_to_namespace_session (&nss);

	wpl_scene *main;
	if (!(main = find_scene("main"))) {
		throw runtime_error("Could not find 'SCENE main'");
	}

	wpl_value_int return_value;

	wpl_block_state program_state(NULL, io, this);

	unique_ptr<wpl_state> main_state(main->new_state(&program_state, io));

	ret = main->run(main_state.get(), &return_value);

	return return_value.get();
}
