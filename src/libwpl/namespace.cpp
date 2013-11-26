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

#include "namespace.h"
#include "namespace_session.h"
#include "identifier.h"
#include "scene.h"
#include "types.h"
#include "debug.h"
#include "pragma.h"
#include "function.h"
#include "variable.h"
#include "parseable.h"
#include "template.h"

#include <utility>
#include <list>

int wpl_namespace::id_counter = 0;

void wpl_namespace::new_register_parseable (wpl_parseable *parseable) {
	if (new_find_parseable_no_parent (parseable->get_name())) {
		throw wpl_exception_name_exists();
	}
	new_parseables.emplace_back(parseable);
}
/*void wpl_namespace::new_register_function (wpl_function *function) {
}
void wpl_namespace::new_register_variable (wpl_variable *variable) {
}*/

wpl_parseable *wpl_namespace::new_find_parseable(const char *name) {
	for (unique_ptr<wpl_parseable> &parseable : new_parseables) {
		if (parseable->is_name(name)) {
			return parseable.get();
		}
	}
	if (parent_namespace) {
		return parent_namespace->new_find_parseable(name);
	}
	return NULL;
}

wpl_parseable *wpl_namespace::new_find_parseable_no_parent(const char *name) {
	for (unique_ptr<wpl_parseable> &parseable : new_parseables) {
		if (parseable->is_name(name)) {
			return parseable.get();
		}
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
 * @brief Find a registered pragma by its name
 *
 * @param name NULL-terminated string with the name
 *
 * @return Return the pragma on success or NULL on failure.
 */
wpl_pragma *wpl_namespace::find_pragma (const char *name) {
	for (wpl_pragma *pragma : pragmas) {
		if (strcmp (name, pragma->get_name()) == 0) {
			return pragma;
		}
	}
	if (parent_namespace != NULL) {
		return parent_namespace->find_pragma(name);
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
wpl_scene *wpl_namespace::find_scene (const char *name) {
	for (wpl_scene *scene : scenes) {
		if (strcmp (name, scene->get_name()) == 0) {
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
wpl_template *wpl_namespace::find_template (const char *name) {
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

/**
 * @brief Find a registered parseable by its name
 *
 * @param name NULL-terminated string with the name
 *
 * @return Return the parseable on success or NULL on failure.
 */
wpl_parseable *wpl_namespace::find_parseable (const char *name) {
	for (wpl_parseable *parseable : parseables) {
		if (strcmp (name, parseable->get_name()) == 0) {
			return parseable;
		}
	}
	if (parent_namespace != NULL) {
		return parent_namespace->find_parseable(name);
	}
	return NULL;
}

/**
 * @brief Find a registered complete type by its name
 *
 * @param name NULL-terminated string with the name
 *
 * @return Return the type on success or NULL on failure.
 */
wpl_type_complete *wpl_namespace::find_complete_type (const char *name) {
	for (wpl_type_complete *type : complete_types) {
		if (type->is_name(name)) {
			return type;
		}
	}
	if (parent_namespace != NULL) {
		return parent_namespace->find_complete_type(name);
	}
	return NULL;
}

/**
 * @brief Find a registered incomplete type by its name
 *
 * @param name NULL-terminated string with the name
 *
 * @return Return the type on success or NULL on failure.
 */
wpl_type_incomplete *wpl_namespace::find_incomplete_type (const char *name) {
	for (wpl_type_incomplete *type : incomplete_types) {
		if (type->is_name(name)) {
			return type;
		}
	}
	if (parent_namespace != NULL) {
		return parent_namespace->find_incomplete_type(name);
	}
	return NULL;
}

/**
 * @brief Find a registered template type by its name
 *
 * @param name NULL-terminated string with the name
 *
 * @return Return the type on success or NULL on failure.
 */
wpl_type_template *wpl_namespace::find_template_type (const char *name) {
	for (wpl_type_template *type : template_types) {
		if (type->is_name(name)) {
			return type;
		}
	}
	if (parent_namespace != NULL) {
		return parent_namespace->find_template_type(name);
	}
	return NULL;
}

void wpl_namespace::push_complete_type(wpl_type_complete *complete_type) {
	complete_types.push_back(complete_type);
}

void wpl_namespace::register_identifier(wpl_type_complete *complete_type) {
#ifdef WPL_DEBUG_NAMESPACE
	DBG("NS (" << this << "): Register complete type '" << complete_type->get_name() << "'" << endl);
#endif
	wpl_identifier *tmp = new wpl_suicidal_holder(complete_type, complete_type->get_name());
	identifiers.emplace_back(tmp);

	if (find_identifier_no_parent (complete_type->get_name()) != tmp) {
		snprintf (exception_msg, exception_msg_length,
			"Name '%s' was already defined in namespace %p", complete_type->get_name(), this);
		THROW_RUNTIME_EXCEPTION(exception_msg);
	}

	complete_types.push_back(complete_type);
}

void wpl_namespace::register_identifier(wpl_type_incomplete *incomplete_type) {
#ifdef WPL_DEBUG_NAMESPACE
	DBG("NS (" << this << "): Register incomplete type '" << incomplete_type->get_name() << "'" << endl);
#endif
	wpl_identifier *tmp = new wpl_suicidal_holder(incomplete_type, incomplete_type->get_name());
	identifiers.emplace_back(tmp);

	if (find_identifier_no_parent (incomplete_type->get_name()) != tmp) {
		snprintf (exception_msg, exception_msg_length,
			"Name '%s' was already defined in namespace %p", incomplete_type->get_name(), this);
		THROW_RUNTIME_EXCEPTION(exception_msg);
	}

	incomplete_types.push_back(incomplete_type);
}

void wpl_namespace::register_identifier(wpl_type_template *template_type) {
#ifdef WPL_DEBUG_NAMESPACE
	DBG("NS (" << this << "): Register template type '" << template_type->get_name() << "'" << endl);
#endif
	wpl_identifier *tmp = new wpl_suicidal_holder(template_type, template_type->get_name());
	identifiers.emplace_back(tmp);

	if (find_identifier_no_parent (template_type->get_name()) != tmp) {
		snprintf (exception_msg, exception_msg_length,
			"Name '%s' was already defined in namespace %p", template_type->get_name(), this);
		THROW_RUNTIME_EXCEPTION(exception_msg);
	}
	template_types.push_back(template_type);
}

void wpl_namespace::register_identifier(wpl_pragma *pragma) {
#ifdef WPL_DEBUG_NAMESPACE
	DBG("NS (" << this << "): Register pragma '" << pragma->get_name() << "'" << endl);
#endif
	identifiers.emplace_back(pragma);

	if (find_identifier_no_parent (pragma->get_name()) != pragma) {
		snprintf (exception_msg, exception_msg_length,
			"Name '%s' was already defined in namespace %p", pragma->get_name(), this);
		THROW_RUNTIME_EXCEPTION(exception_msg);
	}

	pragmas.push_back(pragma);
}

void wpl_namespace::register_identifier(wpl_scene *scene) {
#ifdef WPL_DEBUG_NAMESPACE
	DBG("NS (" << this << "): Register scene '" << scene->get_name() << "'" << endl);
#endif
	identifiers.emplace_back(scene);

	if (find_identifier_no_parent (scene->get_name()) != scene) {
		snprintf (exception_msg, exception_msg_length,
			"Name '%s' was already defined in namespace %p", scene->get_name(), this);
		THROW_RUNTIME_EXCEPTION(exception_msg);
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
		snprintf (exception_msg, exception_msg_length,
			"Name '%s' was already defined in namespace %p", my_template->get_name(), this);
		THROW_RUNTIME_EXCEPTION(exception_msg);
	}

	templates.push_back(my_template);
}

void wpl_namespace::register_identifier(wpl_function *function) {
#ifdef WPL_DEBUG_NAMESPACE
	DBG("NS (" << this << "): Register function '" << function->get_name() << "'" << endl);
#endif
	identifiers.emplace_back(function);

	if (find_identifier_no_parent (function->get_name()) != function) {
		snprintf (exception_msg, exception_msg_length,
			"Name '%s' was already defined in namespace %p", function->get_name(), this);
		THROW_RUNTIME_EXCEPTION(exception_msg);
	}

	functions.push_back(function);
}

void wpl_namespace::register_identifier_hard (wpl_function *function) {
	functions.push_back(function);
}

void wpl_namespace::register_identifier_hard (wpl_variable *variable) {
	variables.emplace_back(variable);
}

void wpl_namespace::register_identifier(wpl_variable *variable) {
#ifdef WPL_DEBUG_NAMESPACE
	DBG("NS (" << this << "): Register variable '" << variable->get_name() << "'" << endl);
#endif
	if (find_nonstatic_variable (variable->get_name()) || find_static_variable (variable->get_name())) {
		snprintf (exception_msg, exception_msg_length,
			"Name '%s' was already defined in namespace %p", variable->get_name(), this);
		THROW_RUNTIME_EXCEPTION(exception_msg);
	}

	variables.emplace_back(variable->clone());
}

void wpl_namespace::register_identifier (wpl_parseable *parseable) {
#ifdef WPL_DEBUG_NAMESPACE
	DBG("NS (" << this << "): Register parseable '" << parseable->get_name() << "'" << endl);
#endif
	identifiers.emplace_back(parseable);

	if (find_identifier_no_parent (parseable->get_name()) != parseable) {
		snprintf (exception_msg, exception_msg_length,
			"Name '%s' was already defined in namespace %p", parseable->get_name(), this);
		THROW_RUNTIME_EXCEPTION(exception_msg);
	}

	parseables.push_back(parseable);
}

void wpl_namespace::copy_variables_to_namespace_session (wpl_namespace_session *receiver) const {
	for (const shared_ptr<wpl_variable> &variable : variables) {
		unique_ptr<wpl_variable> new_variable(variable->clone());
		receiver->push(new_variable.release());
	}
}
















