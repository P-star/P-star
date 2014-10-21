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

#pragma once

#include "namespace.h"
#include "variable.h"

#include <vector>
#include <list>
#include <memory>

enum {
	WPL_NSS_CTX_NONE,
	WPL_NSS_CTX_CHILD,
	WPL_NSS_CTX_SELF,
	WPL_NSS_CTX_FRIEND,
	WPL_NSS_CTX_OUTSIDE
};

class wpl_state;
class wpl_scene;
class wpl_template;
class wpl_expression_state;
class wpl_value_unresolved_identifier;
class wpl_type_complete;

/**
 * @brief This class is passed into all runables which are called with the run() function. The runables copy their identifiers into this temporary namespace. The namespace session creates a child of itself when a runable calls another runable.
 */
class wpl_namespace_session {
	private:
	wpl_namespace_session *parent;
	wpl_namespace_session *nss_this;

	const wpl_namespace *template_namespace;
	int parent_nss_context;
	bool this_and_parent;

	vector<unique_ptr<wpl_variable>> variables_ptr;

	public:
	wpl_namespace_session(const wpl_namespace_session &copy);
	wpl_namespace_session &operator= (const wpl_namespace_session &rhs);
	wpl_namespace_session(
		wpl_namespace_session *parent
	);
	wpl_namespace_session(
		wpl_namespace_session *parent,
		const wpl_namespace *template_namespace
	);
	wpl_namespace_session(
		wpl_namespace_session *parent,
		const wpl_namespace *template_namespace,
		int parent_access_context
	);
	wpl_namespace_session(
		wpl_namespace_session *parent,
		wpl_namespace_session *nss_this,
		const wpl_namespace *template_namespace,
		int parent_access_context
	);
	wpl_namespace_session(const wpl_namespace *template_namespace);
	wpl_namespace_session();
	~wpl_namespace_session();

	void notify_destructors(wpl_state *state);

	void use_this_and_parent() {
		this_and_parent = true;
	}

	wpl_namespace_session *get_parent() {
		return parent;
	}

	bool set_variables_from_expression (wpl_expression_state *exp_state, int discard_pos);
	void replace_variables (wpl_namespace_session *source);
	void reset_variables();

	void push(wpl_variable *variable);

	wpl_variable *find_variable(const wpl_value *return_value);
	virtual wpl_variable *find_variable(const char *name, int ctx);
	wpl_function *find_function_no_parent(const char *name, int ctx);
	wpl_template *find_template (const char *name);
	wpl_scene *find_scene (const char *name);
	const wpl_type_complete *find_complete_type (const char *name);

	virtual int do_operator_on_unresolved (
		wpl_value_unresolved_identifier *unresolved,
		wpl_expression_state *exp_state,
		wpl_value *final_result,
		int ctx
	);

	wpl_variable *get_variable(int i);
	int variable_index(const char *name);

	void variable_list(vector<wpl_variable*> &target) {
		for (unique_ptr<wpl_variable> &variable : variables_ptr) {
			target.push_back(variable.get());
		}
	}
};

