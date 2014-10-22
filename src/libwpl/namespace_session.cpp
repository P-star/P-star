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

#include "namespace_session.h"
#include "expression_state.h"
#include "value.h"
#include "value_unresolved.h"
#include "value_function_ptr.h"
#include "function.h"
#include "variable.h"

void wpl_namespace_session::notify_destructors(wpl_state *state) {
	for (auto it = variables_ptr.rbegin(); it != variables_ptr.rend(); it++) {
		wpl_variable *var = (*it).get();
		var->get_value()->notify_destructor(state, this, state->get_io());
	}
}

void wpl_namespace_session::replace_variables (wpl_namespace_session *source) {
	vector<wpl_variable*> variables;
	source->variable_list(variables);

	if (variables.size() != variables_ptr.size()) {
		throw runtime_error("Size mismatch in wpl_namespace_session::replace_variables (BUG)");
	}

	for (int i = 0; i < variables.size(); i++) {
		wpl_value *source = variables[i]->get_value();

		if (source->isArray() || source->isStruct()) {
			if (!variables_ptr[i]->set_strong(source)) {
				throw runtime_error("Strong set failed in wpl_namespace_session::replace_variables (BUG)");
			}
		}
		else {
			variables_ptr[i]->set_weak(source);
		}
	}
}

void wpl_namespace_session::reset_variables() {
	for (int i = 0; i < variables_ptr.size(); i++) {
		wpl_value *value = variables_ptr[i]->get_value();

		value->reset();
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
	nss_this = rhs.nss_this;
	this_and_parent = rhs.this_and_parent;

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
	this->nss_this = NULL;
	this->parent_nss_context = WPL_NSS_CTX_CHILD;
	this->this_and_parent = false;
}

wpl_namespace_session::wpl_namespace_session(
		const wpl_namespace *template_namespace
) {
	this->template_namespace = template_namespace;
	this->parent = NULL;
	this->nss_this = NULL;
	this->parent_nss_context = WPL_NSS_CTX_CHILD;
	this->this_and_parent = false;

	template_namespace->copy_variables_to_namespace_session(this);
}

wpl_namespace_session::wpl_namespace_session(
		wpl_namespace_session *parent,
		const wpl_namespace *template_namespace
) {
	this->parent = parent;
	this->nss_this = NULL;
	this->template_namespace = template_namespace;
	this->parent_nss_context = WPL_NSS_CTX_CHILD;
	this->this_and_parent = false;

	template_namespace->copy_variables_to_namespace_session(this);
}

wpl_namespace_session::wpl_namespace_session (
		wpl_namespace_session *parent,
		const wpl_namespace *template_namespace,
		int parent_access_context
) {
	this->parent = parent;
	this->nss_this = NULL;
	this->template_namespace = template_namespace;
	this->parent_nss_context = parent_access_context;
	this->this_and_parent = false;

	template_namespace->copy_variables_to_namespace_session(this);
}

wpl_namespace_session::wpl_namespace_session (
		wpl_namespace_session *parent,
		wpl_namespace_session *nss_this,
		const wpl_namespace *template_namespace,
		int parent_access_context
) {
	this->parent = parent;
	this->nss_this = nss_this;
	this->template_namespace = template_namespace;
	this->parent_nss_context = parent_access_context;
	this->this_and_parent = false;

	template_namespace->copy_variables_to_namespace_session(this);
}

wpl_namespace_session::wpl_namespace_session() {
	this->template_namespace = NULL;
	this->parent = NULL;
	this->nss_this = NULL;
	this->parent_nss_context = WPL_NSS_CTX_CHILD;
	this->this_and_parent = false;
}

wpl_namespace_session::~wpl_namespace_session() {
	vector<unique_ptr<wpl_variable>>::reverse_iterator it =
		variables_ptr.rbegin();
	for (; it != variables_ptr.rend(); it++) {
//		cerr << "NS(" << (void*)this << ") destroy variable " << (void*) (*it).get() << " " << (*it)->get_name() << endl;
		(*it).reset(NULL);
	}
}

void wpl_namespace_session::push (wpl_variable *variable) {
//	cerr << "NS(" << (void*)this << ") push variable " << (void*) variable << " " << variable->get_name() << endl;
	this->variables_ptr.emplace_back(variable);
}

/* XXX
   TODO
   Improve error message
   */
void check_identifier_ctx (wpl_identifier_access_holder *identifier, int ctx) {
	int var_flags = identifier->get_access_flags();

	if (
		(ctx == WPL_NSS_CTX_SELF) ||
		(ctx == WPL_NSS_CTX_FRIEND) ||
		(var_flags == WPL_VARIABLE_ACCESS_PUBLIC)
	) {
		return;
	}
	else if (ctx == WPL_NSS_CTX_CHILD) {
		if (var_flags == WPL_VARIABLE_ACCESS_PRIVATE) {
			cerr << "While accessing identifier " << identifier->get_name() <<
				" from child context:\n";
			throw runtime_error("Cannot access private identifier from this context");
		}
		return;
	}
	else if (ctx == WPL_NSS_CTX_OUTSIDE) {
		if (var_flags != WPL_VARIABLE_ACCESS_PUBLIC) {
			cerr << "While accessing identifier " << identifier->get_name() <<
				" from outside context:\n";
			throw runtime_error("Cannot access private or protected identifier from this context");
		}
		return;
	}
	cerr << "identifier was " << identifier->get_name() << endl;
	cerr << "identifier flags was " << var_flags << endl;
	cerr << "ctx was " << ctx << endl;
	throw runtime_error("Unhandled identifier context");
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
			check_identifier_ctx(variable.get(), ctx);
			return variable.get();
		}
	}
   
	wpl_variable *tmp;

	// Search for local static variable
/*	XXX DISABLED, don't need this yet
	if (template_namespace && (tmp = template_namespace->find_static_variable(name))) {
		return tmp;
	}*/

	/*
	   Search in nss_this namespace, which is our friend. In function
	   calls, the nss_this of the function is the block in which the function is defined.
	   */
	if (nss_this && (tmp = nss_this->find_variable(name, WPL_NSS_CTX_FRIEND))) {
		return tmp;
	}
	else if (	((!nss_this) ||
			(nss_this && this_and_parent)) &&
			parent &&
			(tmp = parent->find_variable(name, parent_nss_context))
	) {
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
wpl_function *wpl_namespace_session::find_function_no_parent(const char *name, int ctx) {
	if (template_namespace) {
		wpl_function *function = template_namespace->find_function(name);
		if (function) {
			check_identifier_ctx(function, ctx);
			return function;
		}
	}
	return NULL;
}

wpl_scene *wpl_namespace_session::find_scene(const char *name) {
	if (!template_namespace) {
		return NULL;
	}
	return template_namespace->find_scene(name);
}

wpl_template *wpl_namespace_session::find_template(const char *name) {
	if (!template_namespace) {
		return NULL;
	}
	return template_namespace->find_template(name);
}

const wpl_type_complete *wpl_namespace_session::find_complete_type(const char *name) {
	const wpl_type_complete *ret;

	/* Look everywhere to find the type */
	if (nss_this && (ret = nss_this->find_complete_type(name))) {
		return ret;
	}
	if (template_namespace && (ret = template_namespace->find_complete_type(name))) {
		return ret;
	}
	if (parent && (ret = parent->find_complete_type(name))) {
		return ret;
	}

	return NULL;
}

int wpl_namespace_session::do_operator_on_unresolved (
	wpl_value_unresolved_identifier *unresolved,
	wpl_expression_state *exp_state,
	wpl_value *final_result,
	int ctx
) {
	wpl_function *function = find_function_no_parent(unresolved->get().c_str(), ctx);

	if (!function) {
		if (parent) {
			return parent->do_operator_on_unresolved(
				unresolved,
				exp_state,
				final_result,
				ctx
			);
		}
		return WPL_OP_NAME_UNRESOLVED;
	}

	wpl_value_function_ptr function_ptr(function, NULL, exp_state);
	return function_ptr.do_operator_recursive(exp_state, final_result);
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
