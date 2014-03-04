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

#include "type_file.h"
#include "wpl_file.h"
#include "value_line.h"

#include "value_string.h"

#include "types.h"
#include "exception.h"
#include "function.h"

class wpl_file_open : public wpl_function {
	private:
	ios_base::openmode mode;

	public:
	wpl_file_open(const char *name, ios_base::openmode mode) :
		mode(mode),
		wpl_function(wpl_type_global_bool, name, WPL_VARIABLE_ACCESS_PUBLIC)
	{
		wpl_variable_holder sql("filename", new wpl_value_string(), WPL_VARIABLE_ACCESS_PRIVATE);
		register_identifier(&sql);
	}
	int run (wpl_state *state, wpl_value *final_result);
};

int wpl_file_open::run (
	wpl_state *state,
	wpl_value *final_result
) {
	wpl_variable *this_var;
	wpl_variable *filename_var;

	wpl_function_state *function_state = (wpl_function_state*) state;

	if (!(this_var = function_state->find_variable("this", WPL_NSS_CTX_SELF))) {
		throw runtime_error("FILE error: open(): Could not find 'this' variable");
	}
	if (!(filename_var = function_state->find_variable("filename", WPL_NSS_CTX_SELF))) {
		throw runtime_error("FILE error: open(): Could not find 'filename' variable");
	}

	wpl_value_file *value_file = (wpl_value_file*) this_var->get_value();
	wpl_value_string *filename = dynamic_cast<wpl_value_string*>(filename_var->get_value());
	if (!filename) {
		ostringstream msg;
		msg << "Argument to open() was of type " << filename_var->get_value()->get_type_name() <<
			", but type string was expected\n";
		throw runtime_error(msg.str());
	}
	wpl_file *file = value_file->get_file();

	((wpl_value_bool*) final_result)->set(file->open(filename->toString().c_str(), mode));

	return WPL_OP_OK;
}

class wpl_file_close : public wpl_function {
	public:
	wpl_file_close() :
		wpl_function(wpl_type_global_bool, "close", WPL_VARIABLE_ACCESS_PUBLIC)
	{}
	int run (wpl_state *state, wpl_value *final_result);
};

int wpl_file_close::run (
	wpl_state *state,
	wpl_value *final_result
) {
	wpl_variable *this_var;
	wpl_variable *filename_var;

	wpl_function_state *function_state = (wpl_function_state*) state;

	if (!(this_var = function_state->find_variable("this", WPL_NSS_CTX_SELF))) {
		throw runtime_error("FILE error: close(): Could not find 'this' variable");
	}

	wpl_value_file *value_file = (wpl_value_file*) this_var->get_value();
	wpl_file *file = value_file->get_file();

	((wpl_value_bool*) final_result)->set(file->close());

	return WPL_OP_OK;
}

class wpl_file_error : public wpl_function {
	public:
	wpl_file_error() :
		wpl_function(wpl_type_global_string, "error", WPL_VARIABLE_ACCESS_PUBLIC)
	{}
	int run (wpl_state *state, wpl_value *final_result);
};

int wpl_file_error::run (
	wpl_state *state,
	wpl_value *final_result
) {
	wpl_variable *this_var;

	wpl_function_state *function_state = (wpl_function_state*) state;

	if (!(this_var = function_state->find_variable("this", WPL_NSS_CTX_SELF))) {
		throw runtime_error("FILE error: error(): Could not find 'this' variable");
	}

	wpl_value_file *value_file = (wpl_value_file*) this_var->get_value();
	wpl_file *file = value_file->get_file();

	((wpl_value_string*) final_result)->set(file->get_error().c_str());

	return WPL_OP_OK;
}

class wpl_file_update : public wpl_function {
	public:
	wpl_file_update() :
		wpl_function(wpl_type_global_bool, "update", WPL_VARIABLE_ACCESS_PUBLIC)
	{
		wpl_variable_holder line("line", new wpl_value_line(), WPL_VARIABLE_ACCESS_PRIVATE);
		register_identifier(&line);
	}
	int run (wpl_state *state, wpl_value *final_result);
};

int wpl_file_update::run (
	wpl_state *state,
	wpl_value *final_result
) {
	wpl_variable *this_var;
	wpl_variable *line_var;

	wpl_function_state *function_state = (wpl_function_state*) state;

	if (!(this_var = function_state->find_variable("this", WPL_NSS_CTX_SELF))) {
		throw runtime_error("FILE error: update(): Could not find 'this' variable");
	}
	if (!(line_var = function_state->find_variable("line", WPL_NSS_CTX_SELF))) {
		throw runtime_error("FILE error: update(): Could not find 'line' variable");
	}

	wpl_value_file *value_file = (wpl_value_file*) this_var->get_value();
	wpl_file *file = value_file->get_file();

	wpl_value_line *line = dynamic_cast<wpl_value_line*>(line_var->get_value());
	if (!line) {
		ostringstream msg;
		msg << "Argument to update() was of type " << line_var->get_value()->get_type_name() <<
			", but type LINE was expected\n";
		throw runtime_error(msg.str());
	}

	((wpl_value_bool*) final_result)->set(file->update(line));

	return WPL_OP_OK;
}

class wpl_file_flush : public wpl_function {
	public:
	wpl_file_flush() :
		wpl_function(wpl_type_global_bool, "flush", WPL_VARIABLE_ACCESS_PUBLIC)
	{}
	int run (wpl_state *state, wpl_value *final_result);
};

int wpl_file_flush::run (
	wpl_state *state,
	wpl_value *final_result
) {
	wpl_variable *this_var;

	wpl_function_state *function_state = (wpl_function_state*) state;

	if (!(this_var = function_state->find_variable("this", WPL_NSS_CTX_SELF))) {
		throw runtime_error("FILE error: flush(): Could not find 'this' variable");
	}

	wpl_value_file *value_file = (wpl_value_file*) this_var->get_value();
	wpl_file *file = value_file->get_file();

	((wpl_value_bool*) final_result)->set(file->flush());

	return WPL_OP_OK;
}

wpl_type_file::wpl_type_file() :
	wpl_struct(wpl_typename_file, true)
{
	wpl_variable_holder this_var("this", new wpl_value_file(), WPL_VARIABLE_ACCESS_PRIVATE);
	register_identifier(&this_var);

	register_identifier(new wpl_file_open("open", ios_base::in));
	register_identifier(new wpl_file_open("open_ro", ios_base::in));
	register_identifier(new wpl_file_open("open_rw", ios_base::in|ios_base::out));
	register_identifier(new wpl_file_open("open_truncate", ios_base::in|ios_base::out|ios_base::trunc));
	register_identifier(new wpl_file_close());
	register_identifier(new wpl_file_error());
	register_identifier(new wpl_file_update());
	register_identifier(new wpl_file_flush());
}

wpl_type_file constant_type_file;

void wpl_type_file_register (wpl_namespace *target) {
	target->new_register_parseable(&constant_type_file);
}
