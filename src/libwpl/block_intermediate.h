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

#include "block_intermediate_state.h"
#include "namespace.h"
#include "runable.h"

#include <memory>

/**
 * @brief This class is an intermediate namespace between conditional blocks like if and while and the blocks where they are defined to hold namespace for variables declared in the condition statement.
 */
class wpl_block_intermediate : public wpl_namespace, public wpl_runable {
	private:
	unique_ptr<wpl_runable> runable;

	protected:
	void set_runable(wpl_runable *runable) {
		this->runable.reset(runable);
	}

	public:
	virtual ~wpl_block_intermediate() {}
	virtual int run (wpl_state *state, wpl_value *final_result) override {
		wpl_block_intermediate_state *my_state =
			(wpl_block_intermediate_state*) state;

		return my_state->run_runable(runable.get(), final_result);
	}
	virtual wpl_state *new_state(wpl_namespace_session *nss, wpl_io *io) override {
		return new wpl_block_intermediate_state(nss, io, this);
	}
};
