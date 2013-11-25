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

#include "namespace_session.h"
#include "expression_state.h"
#include "value.h"

void wpl_namespace_session::replace_variables (wpl_namespace_session *source) {
	list<wpl_variable*> variables;
	source->variable_list(variables);

	variables_ptr.clear();
	variables_ptr.reserve(variables.size());

	for (wpl_variable *variable : variables) {
		push (variable->clone());
	}
}

bool wpl_namespace_session::set_variables_from_expression (wpl_expression_state *exp_state, int discard_pos) {

	discard_pos++;

	int discard_length = exp_state->get_discard().size() - discard_pos;

	if (variables_ptr.size() != discard_length) {
#ifdef WPL_DEBUG_FUNCTIONS
		DBG("NSS: Load length mismatch before load attempts (" << variables_ptr.size() << " vs " << discard_length << ")\n");
		for (int i = discard_pos; i < exp_state->get_discard().size(); i++) {
			wpl_value *value = exp_state->get_discard()[i];
			DBG("- " << value->get_type_name() << endl);
		}
#endif
		return false;
	}

#ifdef WPL_DEBUG_FUNCTIONS
	DBG("NSS: Loading " << discard_length << " arguments to function\n");
#endif

	int i = discard_pos;
	int match_count = 0;
	for (unique_ptr<wpl_variable> &variable_ptr : variables_ptr) {
		wpl_value *value = exp_state->get_discard()[i++];
		wpl_variable *variable = variable_ptr.get();

#ifdef WPL_DEBUG_FUNCTIONS
		DBG("NSS: Load into variable " << variable << "('" << variable->get_name() << "')\n");
#endif

		if (variable->get_value()->isStruct() || variable->get_value()->isArray()) {
#ifdef WPL_DEBUG_FUNCTIONS
			DBG("NSS: Handling struct or array\n");
#endif

			if (!variable->set_strong(value)) {
#ifdef WPL_DEBUG_FUNCTIONS
				DBG("NSS: Strong type check failed for argument " << i << "\n");
#endif
				return false;
			}
		}
		else {
#ifdef WPL_DEBUG_FUNCTIONS
			DBG("NSS: set weak argument to " << value << "('" << value->toString() << "')\n");
#endif
			variable->set_weak(value);
		}
	}

	return true;
}

wpl_namespace_session &wpl_namespace_session::operator= (const wpl_namespace_session &rhs) {
	variables_ptr.clear();
	for (const unique_ptr<wpl_variable> &variable : rhs.variables_ptr) {
		variables_ptr.emplace_back(variable->clone());
	}

	template_namespace = rhs.template_namespace;
	parent = rhs.parent;
	sibling = rhs.sibling;
	do_sibling_lookup = rhs.do_sibling_lookup;

	return *this;
}

wpl_namespace_session::wpl_namespace_session(
		const wpl_namespace_session &copy
) {
	operator=(copy);
}

wpl_namespace_session::wpl_namespace_session(
		wpl_namespace_session *parent
) {
	this->template_namespace = NULL;
	this->parent = parent;
	this->sibling = sibling;
	this->do_sibling_lookup = false;
}

wpl_namespace_session::wpl_namespace_session(
		const wpl_namespace *template_namespace
) {
	this->template_namespace = template_namespace;
	this->parent = NULL;
	this->sibling = sibling;
	this->do_sibling_lookup = false;

	template_namespace->copy_variables_to_namespace_session(this);
}

wpl_namespace_session::wpl_namespace_session(
		wpl_namespace_session *parent,
		const wpl_namespace *template_namespace
) {
	this->parent = parent;
	this->sibling = NULL;
	this->template_namespace = template_namespace;
	this->do_sibling_lookup = false;

	template_namespace->copy_variables_to_namespace_session(this);
}

wpl_namespace_session::wpl_namespace_session (
		wpl_namespace_session *parent,
		wpl_namespace_session *sibling,
		const wpl_namespace *template_namespace
) {
	this->parent = parent;
	this->sibling = sibling;
	this->template_namespace = template_namespace;
	this->do_sibling_lookup = false;

	template_namespace->copy_variables_to_namespace_session(this);
}

wpl_namespace_session::wpl_namespace_session() {
	this->template_namespace = NULL;
	this->parent = NULL;
	this->sibling = NULL;
	this->do_sibling_lookup = false;
}

wpl_namespace_session::~wpl_namespace_session() {
	vector<unique_ptr<wpl_variable>>::reverse_iterator it =
		variables_ptr.rbegin();
	for (; it != variables_ptr.rend(); it++) {
		(*it).reset(NULL);
	}
}

void wpl_namespace_session::push (wpl_variable *variable) {
	this->variables_ptr.emplace_back(variable);
}

/**
 * @brief Find a variable in the current namespace session by its name.
 *
 * @param name The name of the variable
 *
 * @return Return the variable on success or NULL on failure.
 */
wpl_variable *wpl_namespace_session::find_variable(const char *name, int ctx){
	// Search for local variable
	for (unique_ptr<wpl_variable> &variable : variables_ptr) {
		if (variable->is_name(name)) {
			return variable.get();
		}
	}
   
	wpl_variable *tmp;

	// Search for local static variable
/*	XXX DISABLED, don't need this yet
	if (template_namespace && (tmp = template_namespace->find_static_variable(name))) {
		return tmp;
	}*/

	// Search in parent namespace session
	if (parent && (tmp = parent->find_variable(name, WPL_NSS_CTX_CHILD))) {
		return tmp;
	}

	// Search in sibling namespace
	if ((do_sibling_lookup && sibling) && (tmp = sibling->find_variable(name, WPL_NSS_CTX_FRIEND))) {
		return tmp;
	}

	return NULL;
}

/**
 * @brief Get variable index in the current namespace session by its name.
 *
 * @param name The name of the variable
 *
 * @return Return >=0 on success or -1 on fail
 */
int wpl_namespace_session::variable_index (const char *name){
	// Search for local variable
	int index = 0;
	for (unique_ptr<wpl_variable> &variable : variables_ptr) {
		if (variable->is_name(name)) {
			return index;
		}
		index++;
	}

	// Search in parent namespace session
	if (parent) {
		int tmp = parent->variable_index(name);
		if (tmp < 0) {
			return -1;
		}
		return index+tmp;
	}

	return -1;
}

wpl_variable *wpl_namespace_session::get_variable(int index) {
	if (index >= variables_ptr.size()) {
		return (parent->get_variable(index - variables_ptr.size()));
	}
	else {
		return variables_ptr[index].get();
	}
}

/**
 * @brief Find a function in the static namespace
 *
 * @param name Name of the function
 *
 * @return A function with matching name on success, or NULL on failure.
 */
wpl_function *wpl_namespace_session::find_function(const char *name, int ctx) {
	if (template_namespace) {
		return template_namespace->find_function(name);
	}
	else if (parent) {
		return parent->find_function(name, WPL_NSS_CTX_CHILD);
	}
	return NULL;
}

/**
 * @brief Find a variable by the pointer to its value. This is useful to check if two expressions points to the same value.
 *
 * @param return_value The value the variable should point to.
 *
 * @return Return the variable on success or NULL on failure.
 */
wpl_variable *wpl_namespace_session::find_variable (const wpl_value *return_value) {
	// Search for local variabl
	for (const unique_ptr<wpl_variable> &variable : variables_ptr) {
		if (variable->get_value() == return_value) {
			return variable.get();
		}
	}

	// Search in parent namespace session
	if (parent) {
		return parent->find_variable(return_value);
	}
	return NULL;
}
