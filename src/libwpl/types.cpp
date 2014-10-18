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

#include "types.h"
#include "namespace.h"

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
const char *wpl_typename_pointer = "pointer";
const char *wpl_typename_hash = "hash";
const char *wpl_typename_auto = "auto";

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
const char *wpl_typename_double_finalizer = "{double finalizer}";
const char *wpl_typename_unsafe_pointer = "{unsafe pointer}";

#include "struct.h"
#include "class.h"
#include "array.h"
#include "hash.h"
#include "pointer.h"

#define DEFINE_TYPE(name)					\
	static wpl_type_##name constant_type_##name;		\
const wpl_type_##name *wpl_type_global_##name =			\
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
DEFINE_TYPE(pointer);
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
DEFINE_TYPE(auto);

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
#include "value_auto.h"

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
NEW_INSTANCE(auto,0)

wpl_value *wpl_type_void::new_instance() const {
	return new wpl_value_void();
}

#define REGISTER_TYPE(name) \
	name_space->register_parseable(&constant_type_##name); \
	name_space->add_type(&constant_type_##name);

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

	REGISTER_TYPE(array);
	REGISTER_TYPE(pointer);
	REGISTER_TYPE(hash);
	REGISTER_TYPE(auto);

	wpl_type_file_register(name_space);
}

