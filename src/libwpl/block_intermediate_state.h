/*

-------------------------------------------------------------

Copyright (c) MMXIV Atle Solbakken
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

class wpl_io;
class wpl_value;
class wpl_runable;
class wpl_namespace;
class wpl_namespace_session;

class wpl_block_intermediate_state : public wpl_namespace_session, public wpl_state {
	private:
	unique_ptr<wpl_state> block_state;
	wpl_runable *block;

	public:
	wpl_block_intermediate_state (
			wpl_state *parent,
			wpl_namespace_session *nss,
			wpl_io *io,
			wpl_namespace *template_namespace,
			wpl_runable *block
	) :
		wpl_namespace_session(nss, template_namespace, WPL_NSS_CTX_SELF),
		wpl_state(parent, nss, io),
		block(block)
	{}
	virtual ~wpl_block_intermediate_state() {}

	void reset() override {
		block_state->reset();
		wpl_namespace_session::reset_variables();
	}

	int run_block (wpl_value *final_result);
};
