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
	wpl_function_state *function_state = (wpl_function_state*) state;

	wpl_value_file *value_file =
		function_state->find_variable_value<wpl_value_file>("open()", "this");
	wpl_value_string *filename =
		function_state->find_variable_value<wpl_value_string>("open()", "filename");

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
	wpl_function_state *function_state = (wpl_function_state*) state;

	wpl_value_file *value_file =
		function_state->find_variable_value<wpl_value_file>("close()", "this");
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
	wpl_function_state *function_state = (wpl_function_state*) state;

	wpl_value_file *value_file =
		function_state->find_variable_value<wpl_value_file>("error()", "this");
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
	wpl_function_state *function_state = (wpl_function_state*) state;

	wpl_value_file *value_file =
		function_state->find_variable_value<wpl_value_file>("update()", "this");
	wpl_value_line *line =
		function_state->find_variable_value<wpl_value_line>("update()", "line");

	wpl_file *file = value_file->get_file();

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
	wpl_function_state *function_state = (wpl_function_state*) state;

	wpl_value_file *value_file =
		function_state->find_variable_value<wpl_value_file>("flush()", "this");

	wpl_file *file = value_file->get_file();

	((wpl_value_bool*) final_result)->set(file->flush());

	return WPL_OP_OK;
}

class wpl_file_begin : public wpl_function {
	public:
	wpl_file_begin() :
		wpl_function(wpl_type_global_line, "begin", WPL_VARIABLE_ACCESS_PUBLIC)
	{}
	int run (wpl_state *state, wpl_value *final_result);
};

int wpl_file_begin::run (
	wpl_state *state,
	wpl_value *final_result
) {
	wpl_function_state *function_state = (wpl_function_state*) state;

	wpl_value_file *value_file =
		function_state->find_variable_value<wpl_value_file>("begin()", "this");

	wpl_file *file = value_file->get_file();

	((wpl_value_line*) final_result)->set(value_file->get_file_shared_ptr(), file->new_chunk_begin());

	return WPL_OP_OK;
}

class wpl_file_end : public wpl_function {
	public:
	wpl_file_end() :
		wpl_function(wpl_type_global_line, "end", WPL_VARIABLE_ACCESS_PUBLIC)
	{}
	int run (wpl_state *state, wpl_value *final_result);
};

int wpl_file_end::run (
	wpl_state *state,
	wpl_value *final_result
) {
	wpl_function_state *function_state = (wpl_function_state*) state;

	wpl_value_file *value_file =
		function_state->find_variable_value<wpl_value_file>("end()", "this");

	wpl_file *file = value_file->get_file();

	((wpl_value_line*) final_result)->set(value_file->get_file_shared_ptr(), file->new_chunk_end());

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
	register_identifier(new wpl_file_begin());
	register_identifier(new wpl_file_end());
}

wpl_type_file constant_type_file;

void wpl_type_file_register (wpl_namespace *target) {
	target->register_parseable(&constant_type_file);
}
