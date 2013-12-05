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

#include "namespace_session.h"
#include "state.h"

#include <memory>

class wpl_value;
class wpl_runable;
class wpl_namespace;

class wpl_block_state : public wpl_namespace_session, public wpl_state {
	private:
	unique_ptr<wpl_state> child_states[WPL_BLOCK_MAX_CHILDREN];
	unique_ptr<wpl_state> run_condition_state;
	unique_ptr<wpl_state> next_else_if_state;
//	void register_child (wpl_runable *runable, int index);

	public:
	/**
	 * @brief This constructor is used by function blocks
	 *
	 * @param parent Function namespace, function arguments
	 * @param sibling Sibling namespace, namespace where the function is defined in
	 * @param template_namespace Template namespace of block
	 */
	wpl_block_state (
			wpl_namespace_session *parent,
			wpl_io *io,
			wpl_namespace_session *sibling,
			wpl_namespace *template_namespace
	) :
		wpl_namespace_session(parent, sibling, template_namespace, WPL_NSS_CTX_SELF),
		wpl_state(parent, io)
	{}

	/**
	 * @brief This constructor is used by normal blocks
	 *
	 * @param parent Namespace of parent block
	 * @param template_namespace Template namespace of block
	 */
	wpl_block_state (
			wpl_namespace_session *parent,
			wpl_io *io,
			wpl_namespace *template_namespace
	) :
		wpl_namespace_session(parent, template_namespace, WPL_NSS_CTX_SELF),
		wpl_state(parent, io)
	{}
	int run_child (wpl_runable *child, int index, wpl_value *final_result);
	int run_run_condition (wpl_runable *runable, wpl_value *final_result);
	int run_next_else_if (wpl_runable *runable, wpl_value *final_result);
};
