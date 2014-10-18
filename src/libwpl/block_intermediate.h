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
#include "parse_and_run.h"

#include <memory>

/**
 * @brief This class is an intermediate namespace between conditional blocks like if and while and the blocks where they are defined to hold namespace for variables declared in the condition statement.
 */
class wpl_block_intermediate : public wpl_parse_and_run, public wpl_namespace {
	private:
	unique_ptr<wpl_parse_and_run> block;

	protected:
	wpl_parse_and_run *get_block() {
		return block.get();
	}

	public:
	wpl_block_intermediate(wpl_parse_and_run *block) :
		wpl_parse_and_run("BLOCK_INTERMEDIATE"),
		wpl_namespace(),
		block(block)
	{}
	wpl_block_intermediate(const wpl_block_intermediate &copy) :
		wpl_parse_and_run(copy),
		wpl_namespace(),
		block(copy.block->new_instance())
	{}
	virtual ~wpl_block_intermediate() {}
	virtual void suicide() override {
		delete this;
	}
	virtual int run (wpl_state *state, wpl_value *final_result) override {
		wpl_block_intermediate_state *my_state =
			(wpl_block_intermediate_state*) state;

		return my_state->run_block(final_result);
	}
	virtual wpl_state *new_state(wpl_state *parent, wpl_namespace_session *nss, wpl_io *io) override {
		return new wpl_block_intermediate_state(parent, nss, io, this, block.get());
	}
};
