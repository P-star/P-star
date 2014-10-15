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
	unique_ptr<wpl_state> runable_state;

	public:
	wpl_block_intermediate_state (
			wpl_namespace_session *parent,
			wpl_io *io,
			wpl_namespace *template_namespace
	) :
		wpl_namespace_session(parent, template_namespace, WPL_NSS_CTX_SELF),
		wpl_state(parent, io)
	{}
	virtual ~wpl_block_intermediate_state() {}

	void reset() override {
		runable_state->reset();
		wpl_namespace_session::reset_variables();
	}

	int run_runable(wpl_runable *runable, wpl_value *final_result);
};
