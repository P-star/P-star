/*

-------------------------------------------------------------

Copyright (c) MMXIII-MMXIX Atle Solbakken
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

#include "pragma.h"
#include "scene.h"
#include "exception.h"
#include "namespace.h"
#include "variable.h"
#include "value_unsafe_pointer.h"
#include "value_array.h"
#include "value_string.h"
#include "io.h"

#include <set>
#include <sstream>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <stdexcept>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

using namespace boost::interprocess;
using namespace std;

void wpl_pragma::parse_default_end() {
	ignore_whitespace();
	if (!ignore_letter(terminator)) {
		cerr << "While looking for terminator '" << terminator << "' after pragma '" << get_name() << "':\n";
		THROW_ELEMENT_EXCEPTION("Could not find terminator");
	}
}

int wpl_pragma_template_var::run(wpl_state *state, wpl_value *final_result) {
	wpl_pragma_state *pragma_state = (wpl_pragma_state*) state;

	wpl_value_unsafe_pointer value_pointer;
	value_pointer.set_do_finalize();

	wpl_value_array my_array(array_type.get(), wpl_type_global_string);

	/* Don't cache this state as we do many strange things */
	unique_ptr<wpl_expression_state> exp_state(
		(wpl_expression_state*)
		value_expression.get()->new_state(state, state->get_nss(), &state->get_io())
	);

	/* Insert an extra operation to fill our array with ID strings. */
	exp_state->push_extra(&my_array);
	exp_state->push_extra(&OP_REPLACE_DISCARD);

	value_expression->run(exp_state.get(), final_result);

	wpl_state *text_state = pragma_state->get_child_state(my_template, 1);

	my_template->output_json(
		text_state,
		&my_array,
		final_result
	);
	return WPL_OP_OK;
}

void wpl_pragma_fixed_text::parse_value(wpl_namespace *parent_namespace) {
	parse_default_end();
}

int wpl_pragma_fixed_text::run (wpl_state *state, wpl_value *final_result) {
	state->get_io() << text;
	return WPL_OP_OK;
}

void wpl_pragma_template_var::parse_value(wpl_namespace *parent_namespace) {
	char value[WPL_VARNAME_SIZE];

	if (!array_type.get()) {
		array_type.reset(wpl_type_global_array->new_instance(wpl_type_global_string));
	}

	// Find template name
	int len = search (WORD, WHITESPACE, false);
	if (len <= 0) {
		THROW_ELEMENT_EXCEPTION("Expected identifier name after pragma declaration");
	}
	check_varname_length(len);
	get_string (value, len);
	if (!(this->my_template = parent_namespace->find_template(value))){
		revert_string(len);
		THROW_ELEMENT_EXCEPTION("Unknown template identifier in pragma definition");
	}

	wpl_expression *exp = new wpl_expression();
	value_expression.reset(exp);

	exp->load_position(get_position());
	exp->parse_value(parent_namespace);
	load_position(exp->get_position());
}

string wpl_pragma_expression::get_exp_name(wpl_pragma_state *pragma_state) {
	if (!exp.get()) {
		return this->name;
	}

	wpl_value_string name;
	name.set_do_finalize();
	int ret = pragma_state->run_child(exp.get(), 1, &name);

	if (!(ret & WPL_OP_OK)) {
		throw runtime_error("wpl_pragma_template::get_template(): Not OK return from expression");
	}

	return name.toString();
}

wpl_scene *wpl_pragma_scene::get_scene(wpl_pragma_state *pragma_state) {
	string name = get_exp_name(pragma_state);

	wpl_scene *my_scene;
	if (!(my_scene = pragma_state->find_scene(name.c_str()))) {
		ostringstream msg;
		msg << "Could not find scene with name '" << name << "'\n";
		throw runtime_error(msg.str());
	}

	return my_scene;
}

int wpl_pragma_scene::run(wpl_state *state, wpl_value *final_result) {
	wpl_pragma_state *pragma_state = (wpl_pragma_state*) state;
	wpl_scene *my_scene = get_scene(pragma_state);
	return pragma_state->run_child(my_scene, 0, final_result);
}

wpl_template *wpl_pragma_template::get_template(wpl_pragma_state *pragma_state) {
	string name = get_exp_name(pragma_state);

	wpl_template *my_template;
	if (!(my_template = pragma_state->find_template(name.c_str()))) {
		ostringstream msg;
		msg << "Could not find template with name '" << name << "'\n";
		throw runtime_error(msg.str());
	}

	return my_template;
}

int wpl_pragma_template::run(wpl_state *state, wpl_value *final_result) {
	wpl_pragma_state *pragma_state = (wpl_pragma_state*) state;
	wpl_template *my_template = get_template(pragma_state);
	return pragma_state->run_child(my_template, 0, final_result);
}

void wpl_pragma_expression::parse_value(wpl_namespace *parent_namespace) {
	char value[WPL_VARNAME_SIZE];
	ignore_whitespace();
	if (ignore_letter('@')) {
		// Find HTML template at run time (dynamic)
		exp.reset(new wpl_expression());
		exp->load_position(get_position());
		exp->parse_value(parent_namespace);
		load_position(exp->get_position());
	}
	else {
		// Find HTML template at parse time (static)
		get_word(value);
		name = value;
		parse_default_end();
	}
}

int wpl_pragma_template_as_var::run(wpl_state *state, wpl_value *final_result) {
	wpl_pragma_state *pragma_state = (wpl_pragma_state*) state;

	wpl_template *my_template = get_template(pragma_state);
	if (!my_template) {
		throw runtime_error("wpl_pragma_template_as_var::run(): No template set");
	}

	wpl_text_var_io_method run_wrapper(my_template);
	return pragma_state->run_child(&run_wrapper, 0, final_result);
}


int wpl_pragma_dump_file::run (wpl_state *state, wpl_value *final_result) {
	wpl_pragma_state *pragma_state = (wpl_pragma_state*) state;

	string filename;
	wpl_expression *expression = value_expression.get();

	if (expression) {
		wpl_value_string result;
		result.set_do_finalize();
		pragma_state->run_child(expression, 0, &result);

		filename = result.toString();
	}
	else {
		filename = static_filename;
	}

	wpl_io &io = state->get_io();

	try {
		file_mapping fm(filename.c_str(), read_only);
		mapped_region region(fm, read_only);

		char *memory = (char*) region.get_address();
		int size = region.get_size();

		io.write(memory, size);
	}
	catch (boost::interprocess::interprocess_exception &e) {
		string msg;
		msg = "Could not open file '" + filename + "' in FILE_DUMP pragma: " + e.what() + "\n";
		throw runtime_error(msg);
	}

	return WPL_OP_NO_RETURN;
}

void wpl_pragma_dump_file::parse_value(wpl_namespace *parent_namespace) {
	ignore_whitespace();
	if (ignore_letter('@')) {
		// Find filename template at run time (dynamic)
		value_expression.reset(new wpl_expression());
		value_expression->load_position(get_position());
		value_expression->parse_value(parent_namespace);
		load_position(value_expression->get_position());
	}
	else {
		// Find filename at parse time (static)
		const char *start = get_string_pointer();
		if (!ignore_string_match(NON_SEMICOLON, 0)) {
			THROW_ELEMENT_EXCEPTION("Expected value after pragma definition");
		}
		const char *end = get_string_pointer();

		parse_default_end();

		static_filename.append(start, (end-start));
	}
}

int wpl_pragma_text_content_type::run (wpl_state *state, wpl_value *final_result) {
	wpl_io &io = state->get_io();
	string buf;
	add_to_string(buf);
	io.http_header("Content-type", buf.c_str());
	return WPL_OP_NO_RETURN;
}

int wpl_pragma_text_http_error::run (wpl_state *state, wpl_value *final_result) {
	wpl_io &io = state->get_io();
	string buf = get_exp_name((wpl_pragma_state*) state);
	io.error(buf.c_str());
	throw runtime_error("Exiting program in HTTP_ERROR pragma");
}

const char wpl_pragma_json_begin::content_type[] = "application/json";
const char wpl_pragma_json_begin::start[] = "{\n";

wpl_variable *wpl_pragma_json_dump::get_variable(wpl_pragma_state *pragma_state) {
	string name = get_exp_name(pragma_state);

	wpl_variable *variable;
	if (!(variable = pragma_state->find_variable(name.c_str(), WPL_NSS_CTX_SELF))) {
		ostringstream msg;
		msg << "Could not find variable with name '" << name << "'\n";
		throw runtime_error(msg.str());
	}

	return variable;
}

int wpl_pragma_json_dump::run(wpl_state *state, wpl_value *final_result) {
	wpl_pragma_state *pragma_state = (wpl_pragma_state*) state;
	wpl_variable *variable = get_variable(pragma_state);
	wpl_value *value = variable->get_value();
	value->output_json(state->get_io());

	return WPL_OP_NO_RETURN;
}

void wpl_pragma_text::parse_value (wpl_namespace *parent_namespace) {
	ignore_string_match(WHITESPACE, 0);
	const char *start = get_string_pointer();

	int ws_count = 0;
	while (char c = get_letter()) {
		if (c == terminator) {
			break;
		}
		else if (M_WHITESPACE(c)) {
			ws_count++;
		}
		else {
			ws_count = 0;
		}
	}

	revert_string(1);

	const char *end = get_string_pointer() - ws_count;

	if (start == end) {
		THROW_ELEMENT_EXCEPTION("Expected value after pragma definition");
	}

	set_text(start, end);

	parse_default_end();
}

#define REGISTER_PRAGMA_BLOCK(classname)				\
	static classname constant_##classname(';');			\
	my_namespace->register_parse_and_run(&constant_##classname);

#define REGISTER_PRAGMA_TEXT(classname)					\
	static classname constant_##classname('}');			\
	my_namespace->register_parse_and_run(&constant_##classname);

void wpl_pragma_add_all_to_namespace (wpl_namespace *my_namespace) {
	REGISTER_PRAGMA_BLOCK(wpl_pragma_text_content_type)
	REGISTER_PRAGMA_BLOCK(wpl_pragma_text_http_error)
	REGISTER_PRAGMA_BLOCK(wpl_pragma_template)
	REGISTER_PRAGMA_BLOCK(wpl_pragma_template_as_var)
	REGISTER_PRAGMA_BLOCK(wpl_pragma_template_var)
	REGISTER_PRAGMA_BLOCK(wpl_pragma_scene)
	REGISTER_PRAGMA_BLOCK(wpl_pragma_json_begin)
	REGISTER_PRAGMA_BLOCK(wpl_pragma_json_end)
	REGISTER_PRAGMA_BLOCK(wpl_pragma_json_dump)
	REGISTER_PRAGMA_BLOCK(wpl_pragma_dump_file)
}
void wpl_pragma_add_template_stuff_to_namespace (wpl_namespace *my_namespace) {
	REGISTER_PRAGMA_TEXT(wpl_pragma_text_content_type)
	REGISTER_PRAGMA_TEXT(wpl_pragma_template)
	REGISTER_PRAGMA_TEXT(wpl_pragma_dump_file)
}
