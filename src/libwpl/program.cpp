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

#include "program.h"
#include "value_array.h"
#include "value_string.h"
#include "value_int.h"

#include "global.h"

#include <utility>
#include <memory>

static mutex init_lock;
static bool initialized = false;

static wpl_global_block constant_global_block;
static wpl_global_text constant_global_text;
static wpl_global_template constant_global_template;

const wpl_global_block *global_block = &constant_global_block;
const wpl_global_text *global_text = &constant_global_text;
const wpl_global_template *global_template = &constant_global_template;

#ifndef WIN32
static list<wpl_module_loader> modules;
#endif

void wpl_types_add_all_to_namespace(wpl_namespace *ns);

wpl_program::wpl_program(wpl_io &io, int argc, char **argv) :
	parser(io, 0)
{
#ifndef WIN32
	{
		lock_guard<mutex> lock(init_lock);
		if (!initialized) {
			char *dummy;
			modules.emplace_back(0, &dummy, "mysql");
			initialized = true;
		}
	}
#endif /* WIN32 */

	wpl_namespace::set_toplevel();

	wpl_types_add_all_to_namespace(this);

	wpl_type_complete *argv_type = wpl_type_global_array->new_instance(wpl_type_global_string);
	add_managed_pointer (argv_type);

	try {
		register_parseable (argv_type);
	}
	catch (wpl_exception_name_exists &e) {
		ostringstream msg;
		msg << "While registering ARGV type with name " <<
			e.get_name() <<
			" in namesapce: Name already defined\n";
		throw runtime_error(msg.str());
	}
	add_type (argv_type);

	unique_ptr<wpl_value_array> val_argv (
			new wpl_value_array (
				argv_type,
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

#ifndef WIN32
	for (wpl_module_loader &module : modules) {
		insert_parent_namespace(module.get_namespace());
	}
#endif
}

void wpl_program::parse_file (const char *filename) {
	parser.parse_file(this, filename);
}

int wpl_program::run(wpl_io &io) {
	int ret;
	wpl_value *value;
	wpl_namespace_session nss;
	copy_variables_to_namespace_session (&nss);

	wpl_scene *main;
	if (!(main = find_scene("main"))) {
		throw runtime_error("Could not find 'SCENE main'");
	}

	wpl_value_int return_value;

	wpl_block_state program_state(&io, this);

	unique_ptr<wpl_state> main_state(main->new_state(&program_state, &program_state, &io));

	ret = main->run(main_state.get(), &return_value);

	return return_value.get();
}
