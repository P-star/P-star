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

#include "block_conditional.h"
#include "expression.h"

#include <memory>

class wpl_io;
class wpl_value;
class wpl_state;
class wpl_namespace;
class wpl_parse_and_run;
class wpl_namespace_session;

class wpl_block_foreach : public wpl_block_conditional {
	private:
	const char *name = "foreach";

	protected:
	unique_ptr<wpl_expression> exp_init;

	public:
	wpl_block_foreach(wpl_parse_and_run *block) :
		wpl_block_conditional(block),
		exp_init()
	{}
	wpl_block_foreach(const wpl_block_foreach &copy) :
		wpl_block_conditional(copy),
		exp_init()
	{}
	virtual ~wpl_block_foreach() {}

	const char *get_name() const override {
		return name;
	}

	wpl_block_foreach *new_instance() const override {
		return new wpl_block_foreach(*this);
	}

	wpl_state *new_state (wpl_state *parent, wpl_namespace_session *nss, wpl_io *io) override;
	void parse_value(wpl_namespace *ns);
	int run(wpl_state *state, wpl_value *final_result) override;
};
