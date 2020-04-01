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

#pragma once

#include "parseable.h"
#include "identifier.h"
#include "exception.h"
#include "variable.h"
#include "type.h"

#include <list>
#include <sstream>
#include <memory>

using namespace std;

class wpl_type;
class wpl_function;
class wpl_variable;
class wpl_value;
class wpl_namespace_session;
class wpl_parseable_identifier;
class wpl_parse_and_run;
class wpl_template;
class wpl_scene;

class wpl_type_template;
class wpl_exception_name_exists {
	private:
	string name;
	public:
	wpl_exception_name_exists (string name) :
		name(name)
	{}
	string get_name() {
		return name;
	}
};

class wpl_namespace {
	private:

	int id;

	/*
   	  When set to true, new template types are added here. This is
	  to prevent types from being added to namespace of loadable modules.
	*/
	bool is_toplevel = false;

	list<shared_ptr<wpl_parseable>> managed_pointers;

	/*
	   TODO
	   This is the old system, remove this
	   */
	list<unique_ptr<wpl_identifier>> identifiers;
	list<wpl_identifier*> hidden_identifiers;

	list<wpl_function*> functions;
	list<wpl_template*> templates;
	list<wpl_scene*> scenes;

	list<shared_ptr<wpl_variable>> variables;

	/*
	   TODO
	   Convert old system stuff to the new system where all
	   identifiers are parseables. Memory of parseables should be managed in
	   static variables inside files for each type.
	   - Functions are looked up at run-time and need a separate list (no RAII)
	   - Variables are cloned and need also need a separate list (no RAII)
	   */

	list<wpl_parseable_identifier*> parseables;
	list<wpl_parse_and_run*> parse_and_runs;

	list<wpl_type_complete*> complete_types;
	list<wpl_type_template*> template_types;
	list<wpl_type_incomplete*> incomplete_types;

	wpl_namespace *parent_namespace;

	wpl_identifier *find_identifier_no_parent(const char *name);

	bool is_hidden (const wpl_identifier *identifier) const;

	protected:
	void set_toplevel() {
		is_toplevel = true;
	}

	public:
	wpl_namespace() {
		parent_namespace = NULL;
	}
	wpl_namespace(const wpl_namespace &copy) {
		throw runtime_error("No cloning of namespace");
	}

	void add_managed_pointer (wpl_parseable *parseable) {
		managed_pointers.emplace_back(parseable);
	}

	void add_managed_pointer (shared_ptr<wpl_type_complete> type) {
		managed_pointers.emplace_back(type);
	}

	/*
	   Templates, when being parsed, requires these to be at the 
	   end after added, but we don't need two of them. We add it
	   both here but let namespaces furter up handle the memory
	   to allow the same type to be used together throughout the
	   program.
	   */
	wpl_type_complete *add_unique_complete_type (shared_ptr<wpl_type_complete> type);
	void add_type (wpl_type_complete *type);
	void add_type (wpl_type_template *type);
	void add_type (wpl_type_incomplete *type);

	/* Used in template parsing */
	wpl_type_complete *get_last_complete_type() const {
		return complete_types.back();
	}

	void register_parseable (wpl_parseable_identifier *parseable);
	wpl_parseable_identifier *find_parseable(const char *name) const;
	wpl_parseable_identifier *find_parseable_no_parent(const char *name) const;

	void register_parse_and_run (wpl_parse_and_run *parse_and_run);
	const wpl_parse_and_run *find_parse_and_run(const char *name) const;
	const wpl_parse_and_run *find_parse_and_run_no_parent(const char *name) const;

	wpl_type_complete *find_complete_type(const wpl_type_complete *type_check) const;
	virtual wpl_type_complete *find_complete_type(const char *name) const;

	wpl_type_template *find_template_type(const char *name) const;

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

	void insert_parent_namespace (wpl_namespace *parent) {
		if (parent_namespace) {
			parent_namespace->insert_parent_namespace(parent);
		}
		else {
			parent_namespace = parent;
		}
	}

	void clear_namespace();

	void generate_typename_list (ostringstream &target);

	wpl_scene *find_scene (const char *name) const;
	wpl_template *find_template (const char *name) const;

	wpl_variable *find_variable (const char *name) const;
	wpl_variable *find_static_variable (const char *name) const;
	wpl_variable *find_nonstatic_variable (const char *name);
	wpl_function *find_function (const char *name) const;

	void register_identifier(wpl_scene *scene);
	void register_identifier(wpl_template *my_template);

	void register_identifier_hard(wpl_function *function);
	void register_identifier_hard(wpl_variable *variable);
	void register_identifier(wpl_variable *variable);
	void register_identifier(wpl_function *function);

	void register_hidden_identifier(wpl_function *function); // Used for struct CTOR

	void register_identifier(wpl_parseable_identifier *parseable);

	void copy_variables_to_namespace_session (wpl_namespace_session *receiver) const;
};

