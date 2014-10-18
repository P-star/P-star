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

#include "namespace_session.h"
#include "block_intermediate_state.h"
#include "state.h"

#include <memory>

class wpl_value;
class wpl_runable;
class wpl_namespace;

class wpl_block_conditional_state : public wpl_block_intermediate_state {
	protected:
	unique_ptr<wpl_state> condition_state;

	wpl_block_conditional_state (
			wpl_state *parent,
			wpl_namespace_session *nss,
			wpl_io *io,
			wpl_namespace *template_namespace,
			wpl_runable *runable
	) :
		wpl_block_intermediate_state(parent, nss, io, template_namespace, runable)
	{}
	virtual ~wpl_block_conditional_state() {}

	public:
	int run_condition (wpl_runable *runable, wpl_value *final_result);
};

class wpl_block_if_state : public wpl_block_conditional_state {
	private:
	unique_ptr<wpl_state> next_else_if_state;

	public:
	wpl_block_if_state (
			wpl_state *parent,
			wpl_namespace_session *nss,
			wpl_io *io,
			wpl_namespace *template_namespace,
			wpl_runable *runable
	) :
		wpl_block_conditional_state(parent, nss, io, template_namespace, runable)
	{}
	virtual ~wpl_block_if_state() {}

	int run_next_else_if(wpl_runable *runable, wpl_value *final_result);
};

class wpl_block_while_state : public wpl_block_conditional_state {
	public:
	wpl_block_while_state (
			wpl_state *parent,
			wpl_namespace_session *nss,
			wpl_io *io,
			wpl_namespace *template_namespace,
			wpl_runable *runable
	) :
		wpl_block_conditional_state(parent, nss, io, template_namespace, runable)
	{}
	virtual ~wpl_block_while_state() {}
};

class wpl_block_foreach_state : public wpl_block_conditional_state {
	private:
	unique_ptr<wpl_state> init_state;

	public:
	wpl_block_foreach_state (
			wpl_state *parent,
			wpl_namespace_session *nss,
			wpl_io *io,
			wpl_namespace *template_namespace,
			wpl_runable *runable
	) :
		wpl_block_conditional_state(parent, nss, io, template_namespace, runable)
	{}
	virtual ~wpl_block_foreach_state() {}

	int run_condition (wpl_runable *runable, wpl_value *final_result, int loop_number);
	int run_init (wpl_runable *runable, wpl_value *final_result);
};

class wpl_block_for_state : public wpl_block_foreach_state {
	unique_ptr<wpl_state> continue_state;

	public:
	wpl_block_for_state (
			wpl_state *parent,
			wpl_namespace_session *nss,
			wpl_io *io,
			wpl_namespace *template_namespace,
			wpl_runable *runable
	) :
		wpl_block_foreach_state(parent, nss, io, template_namespace, runable)
	{}
	virtual ~wpl_block_for_state() {}

	int run_continue (wpl_runable *runable, wpl_value *final_result);
};

class wpl_block_state : public wpl_namespace_session, public wpl_state {
	private:
	unique_ptr<wpl_state> child_states[WPL_BLOCK_MAX_CHILDREN];

	int run_count = 0;

	public:
	/**
	 * @brief 
	 *
	 * @param parent Parent state
	 * @param nss Function namespace, function arguments
	 * @param io IO method
	 * @param sibling Sibling namespace, namespace where the function is defined in
	 * @param template_namespace Template namespace of block
	 */
	wpl_block_state (
			wpl_state *parent,
			wpl_namespace_session *nss,
			wpl_io *io,
			wpl_namespace_session *sibling,
			wpl_namespace *template_namespace
	) :
		wpl_namespace_session(nss, sibling, template_namespace, WPL_NSS_CTX_SELF),
		wpl_state(parent, nss, io)
	{}

	/**
	 * @brief 
	 *
	 * @param parent Parent state
	 * @param nss Namespace of parrent block
	 * @param io IO-method
	 * @param template_namespace Template namespace of block
	 */
	wpl_block_state (
			wpl_state *parent,
			wpl_namespace_session *nss,
			wpl_io *io,
			wpl_namespace *template_namespace
	) :
		wpl_namespace_session(nss, template_namespace, WPL_NSS_CTX_SELF),
		wpl_state(parent, nss, io)
	{}

	/**
	 * @brief State used by top-level program 
	 *
	 * @param io IO-method
	 * @param template_namespace Template namespace of block
	 */
	wpl_block_state (
			wpl_io *io,
			wpl_namespace *template_namespace
	) :
		wpl_namespace_session(NULL, template_namespace, WPL_NSS_CTX_SELF),
		wpl_state(io)
	{}

	int run_child (wpl_runable *child, int index, wpl_value *final_result);
	void reset() override;
};
