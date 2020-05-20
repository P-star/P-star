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

#include "namespace.h"
#include "namespace_session.h"
#include "identifier.h"
#include "scene.h"
#include "debug.h"
#include "function.h"
#include "variable.h"
#include "parseable.h"
#include "template.h"

#include <utility>
#include <list>

wpl_type_complete *wpl_namespace::add_unique_complete_type (shared_ptr<wpl_type_complete> type) {
	wpl_type_complete *complete_type;

	/* Add all complete types to the top namespace */
	if (!is_toplevel && parent_namespace) {
		complete_type = parent_namespace->add_unique_complete_type(type);
	}
	else if (!(complete_type = find_complete_type(type.get()))) {
		register_parseable(type.get());
		add_managed_pointer(type);
		complete_type = type.get();
	}

	/* We need to do this even if were already added so that template
	   parsing can find the correct value */
	complete_types.remove(complete_type);
	complete_types.push_back(complete_type);

	return complete_type;
}

void wpl_namespace::add_type (wpl_type_complete *type) {
	complete_types.remove(type);
	complete_types.push_back(type);
}

void wpl_namespace::add_type (wpl_type_template *type) {
	template_types.remove(type);
	template_types.push_back(type);
}

void  wpl_namespace::add_type (wpl_type_incomplete *type) {
	incomplete_types.remove(type);
	incomplete_types.push_back(type);
}

const wpl_parse_and_run *wpl_namespace::find_parse_and_run(const char *name) const {
	if (const wpl_parse_and_run *ret = find_parse_and_run_no_parent(name)) {
		return ret;
	}
	if (parent_namespace) {
		return parent_namespace->find_parse_and_run(name);
	}
	return NULL;
}

const wpl_parse_and_run *wpl_namespace::find_parse_and_run_no_parent(const char *name) const {
	for (const wpl_parse_and_run *parse_and_run : parse_and_runs) {
		if (strcmp(parse_and_run->get_name(), name) == 0) {
			return parse_and_run;
		}
	}
	return NULL;
}

void wpl_namespace::register_parse_and_run (wpl_parse_and_run *parse_and_run) {
	if (find_parse_and_run_no_parent (parse_and_run->get_name())) {
		throw wpl_exception_name_exists(parse_and_run->get_name());
	}
	parse_and_runs.push_back(parse_and_run);
}

wpl_parseable_identifier *wpl_namespace::find_parseable(const char *name) const {
	if (wpl_parseable_identifier *ret = find_parseable_no_parent(name)) {
		return ret;
	}
	if (parent_namespace) {
		return parent_namespace->find_parseable(name);
	}
	return NULL;
}

wpl_parseable_identifier *wpl_namespace::find_parseable_no_parent(const char *name) const {
	for (wpl_parseable_identifier *parseable : parseables) {
		if (parseable->is_name(name) && !is_hidden(parseable)) {
			return parseable;
		}
	}
	return NULL;
}

void wpl_namespace::register_parseable (wpl_parseable_identifier *parseable) {
	if (find_parseable_no_parent (parseable->get_name())) {
		throw wpl_exception_name_exists(parseable->get_name());
	}
	parseables.push_back(parseable);
}

wpl_type_complete *wpl_namespace::find_complete_type(const wpl_type_complete *type_check) const {
	for (auto type : complete_types) {
		if (type->check_type(type_check)) {
			return type;
		}
	}
	if (parent_namespace) {
		return parent_namespace->find_complete_type(type_check);
	}
	return NULL;
}

wpl_type_complete *wpl_namespace::find_complete_type(const char *name) const {
	for (auto type : complete_types) {
		if (type->is_name(name)) {
			return type;
		}
	}
	if (parent_namespace) {
		return parent_namespace->find_complete_type(name);
	}
	return NULL;
}

wpl_type_template *wpl_namespace::find_template_type(const char *name) const {
	for (auto type : template_types) {
		if (type->is_name(name)) {
			return type;
		}
	}
	if (parent_namespace) {
		return parent_namespace->find_template_type(name);
	}
	return NULL;
}

void wpl_namespace::generate_typename_list(ostringstream &target) {
	bool first = true;
	for (shared_ptr<wpl_variable> &variable : variables) {
		target << (!first ? "," : "") << variable->get_type_name();
		first = false;
	}
}

wpl_identifier *wpl_namespace::find_identifier_no_parent (const char *name) {
	for (unique_ptr<wpl_identifier> &identifier : identifiers) {
		if (identifier->is_name(name)) {
			return identifier.get();
		}
	}
	return NULL;
}

/**
 * @brief Find a registered scene by its name
 *
 * @param name NULL-terminated string with the name
 *
 * @return Return the scene on success or NULL on failure.
 */
wpl_scene *wpl_namespace::find_scene (const char *name) const {
	for (wpl_scene *scene : scenes) {
		if (strcmp (name, scene->wpl_identifier::get_name()) == 0) {
			return scene;
		}
	}
	if (parent_namespace != NULL) {
		return parent_namespace->find_scene(name);
	}
	return NULL;
}

/**
 * @brief Find a registered template by its name
 *
 * @param name NULL-terminated string with the name
 *
 * @return Return the template on success or NULL on failure.
 */
wpl_template *wpl_namespace::find_template (const char *name) const {
	for (wpl_template *my_template : templates) {
		if (strcmp (name, my_template->get_name()) == 0) {
			return my_template;
		}
	}
	if (parent_namespace != NULL) {
		return parent_namespace->find_template(name);
	}
	return NULL;
}

/**
 * @brief Find a registered function by its name
 *
 * @param name NULL-terminated string with the name
 *
 * @return Return the function on success or NULL on failure.
 */
wpl_function *wpl_namespace::find_function (const char *name) const {
	for (wpl_function *function : functions) {
		if (strcmp (name, function->get_name()) == 0) {
			return function;
		}
	}
	if (parent_namespace != NULL) {
		return parent_namespace->find_function(name);
	}
	return NULL;
}

/**
 * @brief Find a registered variable, also search parents
 *
 * @param name NULL-terminated string with the name
 *
 * @return Return the variable on success or NULL on failure.
 */
wpl_variable *wpl_namespace::find_variable (const char *name) const {
	for (const shared_ptr<wpl_variable> &variable : variables) {
		if (strcmp (name, variable->get_name()) == 0) {
			return variable.get();
		}
	}
	if (parent_namespace != NULL) {
		return parent_namespace->find_variable(name);
	}
	return NULL;
}

/**
 * @brief Find a registered static variable by its name
 *
 * @param name NULL-terminated string with the name
 *
 * @return Return the variable on success or NULL on failure.
 */
wpl_variable *wpl_namespace::find_static_variable (const char *name) const {
	for (const shared_ptr<wpl_variable> &variable : variables) {
		if (variable->isStatic() && strcmp (name, variable->get_name()) == 0) {
			return variable.get();
		}
	}
	return NULL;
}

/**
 * @brief Find a registered variable by its name
 *
 * @param name NULL-terminated string with the name
 *
 * @return Return the variable on success or NULL on failure.
 */
wpl_variable *wpl_namespace::find_nonstatic_variable (const char *name) {
	for (shared_ptr<wpl_variable> &variable : variables) {
		if ((!variable->isStatic()) && strcmp (name, variable->get_name()) == 0) {
			return variable.get();
		}
	}
	return NULL;
}

void wpl_namespace::register_identifier(wpl_scene *scene) {
#ifdef WPL_DEBUG_NAMESPACE
	DBG("NS (" << this << "): Register scene '" << scene->get_name() << "'" << endl);
#endif
	identifiers.emplace_back(scene);

	if (find_identifier_no_parent (scene->wpl_identifier::get_name()) != scene) {
		ostringstream msg;
		msg << "Name '" << scene->wpl_identifier::get_name() << "' was already defined in namespace " << this;
		throw runtime_error(msg.str());
	}

	scenes.push_back(scene);
}

void wpl_namespace::register_identifier(wpl_template *my_template) {
#ifdef WPL_DEBUG_NAMESPACE
	DBG("NS (" << this << "): Register template '" << my_template->get_name() << "'" << endl);
#endif
	wpl_identifier *tmp = new wpl_suicidal_holder(my_template, my_template->get_name());
	identifiers.emplace_back(tmp);

	if (find_identifier_no_parent (my_template->get_name()) != tmp) {
		ostringstream msg;
		msg << "Name '" << my_template->get_name() << "' was already defined in namespace " << this;
		throw runtime_error(msg.str());
	}

	templates.push_back(my_template);
}

void wpl_namespace::register_identifier(wpl_function *function) {
#ifdef WPL_DEBUG_NAMESPACE
	DBG("NS (" << this << "): Register function '" << function->get_name() << "'" << endl);
#endif
	identifiers.emplace_back(function);

	if (find_identifier_no_parent (function->get_name()) != function) {
		ostringstream msg;
		msg << "Name '" << function->get_name() << "' was already defined in namespace " << this;
		throw runtime_error(msg.str());
	}

	functions.push_back(function);
}

void wpl_namespace::register_hidden_identifier(wpl_function *function) {
#ifdef WPL_DEBUG_NAMESPACE
	DBG("NS (" << this << "): Register hidden function '" << function->get_name() << "'" << endl);
#endif
	hidden_identifiers.emplace_back(function);
	register_identifier(function);
}

void wpl_namespace::register_identifier_hard (wpl_function *function) {
	functions.push_back(function);
}

void wpl_namespace::register_identifier_hard (wpl_variable *variable) {
	variables.emplace_back(variable);
}

void wpl_namespace::register_identifier(wpl_variable *variable) {
#ifdef WPL_DEBUG_NAMESPACE
	DBG("NS (" << this << "): Register variable " << variable << " '" << variable->get_name() << "'" << endl);
#endif
	if (find_nonstatic_variable (variable->get_name()) || find_static_variable (variable->get_name())) {
		ostringstream msg;
		msg << "Name '" << variable->get_name() << "' was already defined in namespace " << this;
		throw runtime_error(msg.str());
	}

	variables.emplace_back(variable->clone());
}

void wpl_namespace::register_identifier (wpl_parseable_identifier *parseable) {
#ifdef WPL_DEBUG_NAMESPACE
	DBG("NS (" << this << "): Register parseable '" << parseable->get_name() << "'" << endl);
#endif
	identifiers.emplace_back(parseable);

	if (find_identifier_no_parent (parseable->get_name()) != parseable) {
		ostringstream msg;
		msg << "Name '" << parseable->get_name() << "' was already defined in namespace " << this;
		throw runtime_error(msg.str());
	}

	parseables.push_back(parseable);
}

void wpl_namespace::copy_variables_to_namespace_session (wpl_namespace_session *receiver) const {
	for (const shared_ptr<wpl_variable> &variable : variables) {
		unique_ptr<wpl_variable> new_variable(variable->clone());
		receiver->push(new_variable.release());
	}
}

bool wpl_namespace::is_hidden (const wpl_identifier *identifier) const {
	for (const wpl_identifier *test : hidden_identifiers) {
		if (test == identifier) {
			return true;
		}
	}
	return false;
}
















