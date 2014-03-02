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

#include "types.h"
#include "namespace.h"

#include "struct.h"
#include "class.h"
#include "array.h"

#include "variable.h"
#include "user_function.h"

#include "value_void.h"
#include "value_string.h"
#include "value_bool.h"
#include "value_int.h"
#include "value_longlongint.h"
#include "value_uint.h"
#include "value_longlonguint.h"
#include "value_float.h"
#include "value_double.h"

#include "value_env.h"
#include "value_get.h"
#include "value_post.h"
#include "value_stdin.h"
#include "value_time.h"
#include "value_line.h"

#include <string>
#include <cstring>
#include <iostream>

using namespace std;

const char *wpl_typename_void = "void";
const char *wpl_typename_bool = "bool";
const char *wpl_typename_int = "int";
const char *wpl_typename_llint = "llint";
const char *wpl_typename_uint = "uint";
const char *wpl_typename_lluint = "lluint";
const char *wpl_typename_float = "float";
const char *wpl_typename_double = "double";
const char *wpl_typename_string = "string";
const char *wpl_typename_vstring = "vstring";
const char *wpl_typename_struct = "struct";
const char *wpl_typename_class = "class";
const char *wpl_typename_array = "array";
const char *wpl_typename_hash = "hash";

const char *wpl_typename_env = "ENV";
const char *wpl_typename_get = "GET";
const char *wpl_typename_post = "POST";
const char *wpl_typename_stdin = "STDIN";
const char *wpl_typename_time = "TIME";
const char *wpl_typename_file = "FILE";
const char *wpl_typename_line = "LINE";

const char *wpl_typename_regex = "{regex}";
const char *wpl_typename_expression = "{expression}";
const char *wpl_typename_wrapper = "{value wrapper}";
const char *wpl_typename_output_trigger = "{output trigger}";
const char *wpl_typename_unresolved_identifier = "{unresolved identifier}";
const char *wpl_typename_function_ptr = "{function pointer}";
const char *wpl_typename_list = "{list}";
const char *wpl_typename_constant_pointer = "{constant pointer}";
const char *wpl_typename_unsafe_pointer = "{unsafe pointer}";

#define DEFINE_TYPE(name)					\
	static wpl_type_##name constant_type_##name;		\
wpl_type_##name *wpl_type_global_##name =			\
	&constant_type_##name;

DEFINE_TYPE(void);
DEFINE_TYPE(int);
DEFINE_TYPE(llint);
DEFINE_TYPE(uint);
DEFINE_TYPE(lluint);
DEFINE_TYPE(float);
DEFINE_TYPE(double);
DEFINE_TYPE(bool);
DEFINE_TYPE(string);
DEFINE_TYPE(array);
DEFINE_TYPE(hash);
DEFINE_TYPE(struct);
DEFINE_TYPE(class);
DEFINE_TYPE(env);
DEFINE_TYPE(get);
DEFINE_TYPE(post);
#ifndef WIN32
DEFINE_TYPE(stdin);
#endif
DEFINE_TYPE(time);
DEFINE_TYPE(line);

void wpl_type_begin_function_declaration::parse_value (wpl_namespace *parent_namespace) {
	wpl_user_function *function = new wpl_user_function(type, name.c_str(), access_flags);
	parent_namespace->register_identifier(function);

	function->load_position(get_position());
	function->parse_value(parent_namespace);
	load_position(function->get_position());
}

void wpl_type_begin_variable_declaration::create_variable (wpl_namespace *parent_namespace) {
	wpl_variable_holder new_var (name.c_str(), type->new_instance(), access_flags);
	parent_namespace->register_identifier(&new_var);
}

void wpl_type_complete::parse_value (wpl_namespace *parent_namespace) {
	wpl_matcher_position begin_pos(get_position());

	ignore_whitespace();
	if (ignore_letter ('>')) {
		throw wpl_type_end_template_declaration(this);
	}

	char name[WPL_VARNAME_SIZE];
	try {
		get_word(name);
	}
	catch (wpl_element_exception &e) {
		cerr << "While parsing complete type '" << get_name() << "':\n";
		throw;
	}

	ignore_whitespace();
	if (ignore_letter ('(')) {
		throw wpl_type_begin_function_declaration(this, name, get_position());
	}
	else {
		throw wpl_type_begin_variable_declaration(this, name, begin_pos);
	}
}

wpl_parseable *wpl_type_template::register_unique_parseable (
		wpl_namespace *parent_namespace,
		const wpl_type_complete *type
) {
	unique_ptr<wpl_type_complete> new_type(new_instance(type));

	// Check if this template type is already defined
	wpl_parseable *parseable = NULL;
	if (!(parseable = parent_namespace->new_find_parseable(new_type->get_name()))) {
		parent_namespace->new_register_parseable(new_type.get());
		parseable = new_type.release();
		parent_namespace->add_managed_pointer(parseable);
	}

	return parseable;
}

void wpl_type_template::parse_value (wpl_namespace *parent_namespace) {
	char buf[WPL_VARNAME_SIZE];

	ignore_whitespace();
	if (!ignore_letter ('<')) {
		THROW_ELEMENT_EXCEPTION("Expected '<' after template type name");
	}

	try {
		get_word(buf);
	}
	catch (wpl_element_exception &e) {
		cerr << "While parsing name for template sub-type:\n";
		throw;
	}

	wpl_parseable *parseable;
	if (!(parseable = parent_namespace->new_find_parseable(buf))) {
		cerr << "While parsing name '" << buf << "' inside template definition:\n";
		THROW_ELEMENT_EXCEPTION("Undefined name");
	}

	try {
		parseable->load_position(get_position());
		parseable->parse_value(parent_namespace);
	}
	catch (wpl_type_end_template_declaration &e) {
		load_position(parseable->get_position());
		parseable = register_unique_parseable(parent_namespace, e.get_type());
		try {
			parseable->load_position(get_position());
			parseable->parse_value(parent_namespace);
		}
		catch (wpl_type_end_template_declaration &e) {
			load_position(parseable->get_position());
			parseable = register_unique_parseable(parent_namespace, e.get_type());

			parseable->load_position(get_position());
			parseable->parse_value(parent_namespace);
		}
	}
}

wpl_type_complete_template::wpl_type_complete_template (
		const wpl_type_template *mother_type,
		const wpl_type_complete *template_type
		) :
	wpl_type_complete((string(mother_type->get_name()) + "<" + template_type->get_name() + ">").c_str())
{
	this->template_type = template_type;
	this->mother_type = mother_type;
}

void wpl_type_incomplete::parse_value (wpl_namespace *parent_namespace) {
	char buf[WPL_VARNAME_SIZE];
	try {
		get_word(buf);
	}
	catch (wpl_element_exception &e) {
		cerr << "While parsing '" << get_name() <<
		       "' definition:\n";
		throw;
	}

	wpl_type_user_incomplete *usr_obj = new_instance(buf);
	parent_namespace->new_register_parseable(usr_obj);
	parent_namespace->add_managed_pointer (usr_obj);

	usr_obj->set_parent_namespace(parent_namespace);
	usr_obj->load_position(get_position());

	usr_obj->parse_value(usr_obj);

	throw runtime_error("Expected end statement exception when parsing incomplete type");
}

#define NEW_INSTANCE(name,default_value)			\
wpl_value *wpl_type_##name::new_instance() const {		\
	wpl_value *ret = new wpl_value_##name(default_value);	\
	return ret;						\
}								\

NEW_INSTANCE(int,0)
NEW_INSTANCE(llint,0)
NEW_INSTANCE(uint,0)
NEW_INSTANCE(lluint,0)
NEW_INSTANCE(float,0.0)
NEW_INSTANCE(double,0.0)
NEW_INSTANCE(bool,false)
NEW_INSTANCE(string,"")
NEW_INSTANCE(env,0)
NEW_INSTANCE(get,0)
NEW_INSTANCE(post,0)
#ifndef WIN32
NEW_INSTANCE(stdin,0)
#endif
NEW_INSTANCE(time,0)
NEW_INSTANCE(line,0)

wpl_value *wpl_type_void::new_instance() const {
	return new wpl_value_void();
}

#define REGISTER_TYPE(name) \
	name_space->new_register_parseable(&constant_type_##name);

// From type_file.cpp
void wpl_type_file_register(wpl_namespace *target);

void wpl_types_add_all_to_namespace(wpl_namespace *name_space) {
	REGISTER_TYPE(void);
	REGISTER_TYPE(int);
	REGISTER_TYPE(llint);
	REGISTER_TYPE(uint);
	REGISTER_TYPE(lluint);
	REGISTER_TYPE(float);
	REGISTER_TYPE(double);
	REGISTER_TYPE(bool);
	REGISTER_TYPE(string);
	REGISTER_TYPE(array);
	REGISTER_TYPE(hash);
	REGISTER_TYPE(struct);
	REGISTER_TYPE(class);
	REGISTER_TYPE(env);
	REGISTER_TYPE(get);
	REGISTER_TYPE(post);
#ifndef WIN32
	REGISTER_TYPE(stdin);
#endif
	REGISTER_TYPE(time);
	REGISTER_TYPE(line);

	wpl_type_file_register(name_space);
}

