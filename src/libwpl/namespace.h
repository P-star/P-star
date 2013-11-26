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

#include "debug.h"
#include "identifier.h"
#include "exception.h"
#include "variable.h"
#include "parseable.h"

#include <list>
#include <sstream>
#include <memory>

using namespace std;

class wpl_type;
class wpl_function;
class wpl_variable;
class wpl_value;
class wpl_type_complete;
class wpl_type_incomplete;
class wpl_type_template;
class wpl_namespace_session;
//class wpl_parseable;
class wpl_template;
class wpl_scene;
class wpl_pragma;

class wpl_exception_name_exists {};

class wpl_namespace {
	private:

	int id;
	static int id_counter;

	/*
	   TODO
	   This is the old system, remove this
	   */
	list<unique_ptr<wpl_identifier>> identifiers;

	list<wpl_function*> functions;
	list<wpl_pragma*> pragmas;
	list<wpl_template*> templates;
	list<wpl_scene*> scenes;
	list<wpl_parseable*> parseables;
	list<wpl_type_complete*> complete_types;
	list<wpl_type_incomplete*> incomplete_types;
	list<wpl_type_template*> template_types;

	list<shared_ptr<wpl_variable>> variables;

	/*
	   TODO
	   Convert old system stuff to the new system where all
	   identifiers are parseables.
	   - Functions are looked up at run-time and need a separate list (no RAII)
	   - Variables are cloned and need also need a separate list (no RAII)
	   */

	list<unique_ptr<wpl_parseable>> new_parseables;
/*	list<wpl_variable*> variables_new;
	list<wpl_function*> functions_new;*/

	wpl_namespace *parent_namespace;

	wpl_identifier *find_identifier_no_parent(const char *name);
//	wpl_identifier *find_identifier(const char *name);

	public:
	wpl_namespace() {
		id = ++id_counter;
		parent_namespace = NULL;
	}
	wpl_namespace(const wpl_namespace &copy) {
		throw runtime_error("No cloning of namespace");
	}

	void new_register_parseable (wpl_parseable *parseable);
/*	void new_register_function (wpl_function *function);
	void new_register_variable (wpl_variable *variable);*/

	wpl_parseable *new_find_parseable(const char *name);
	wpl_parseable *new_find_parseable_no_parent(const char *name);

	int variables_count() const {
		return variables.size();
	}

	int identifiers_count() const {
		return identifiers.size();
	}

	bool namespace_is_empty () const {
		return (identifiers.size() == 0);
	}

	wpl_namespace *get_parent_namespace () {
		return parent_namespace;
	}

	void set_parent_namespace (wpl_namespace *parent) {
		parent_namespace = parent;
	}

	void clear_namespace();

	void generate_typename_list (ostringstream &target);

	wpl_pragma *find_pragma (const char *name);
	wpl_scene *find_scene (const char *name);
	wpl_template *find_template (const char *name);

	wpl_variable *find_variable (const char *name) const;
	wpl_variable *find_static_variable (const char *name) const;
	wpl_variable *find_nonstatic_variable (const char *name);
	wpl_function *find_function (const char *name) const;

	wpl_type_complete *find_complete_type (const char *name);
	wpl_type_incomplete *find_incomplete_type (const char *name);
	wpl_type_template *find_template_type (const char *name);

	wpl_parseable *find_parseable (const char *name);

	void register_identifier(wpl_pragma *pragma);
	void register_identifier(wpl_scene *scene);
	void register_identifier(wpl_template *my_template);

	void register_identifier_hard(wpl_function *function);
	void register_identifier_hard(wpl_variable *variable);
	void register_identifier(wpl_variable *variable);
	void register_identifier(wpl_function *function);

	void push_complete_type(wpl_type_complete *type);
	void register_identifier(wpl_type_complete *type);
	void register_identifier(wpl_type_incomplete *type);
	void register_identifier(wpl_type_template *type);

	void register_identifier(wpl_parseable *parseable);

	void copy_variables_to_namespace_session (wpl_namespace_session *receiver) const;
};

