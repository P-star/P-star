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

#include "block_conditional.h"

class wpl_io;
class wpl_value;
class wpl_state;
class wpl_namespace;
class wpl_namespace_session;

class wpl_block_if : public wpl_block_conditional {
	const char *name = "if";

	public:
	enum {
		F_IF,
		F_ELSE_IF,
		F_ELSE
	};

	const int flag;
	unique_ptr<wpl_block_if> next_else_if;

	public:
	wpl_block_if(wpl_parse_and_run *block, const int flag) :
		wpl_block_conditional(block),
		flag(F_IF)
	{}
	wpl_block_if(wpl_parse_and_run *block) :
		wpl_block_conditional(block),
		flag(F_IF)
	{}
	wpl_block_if(const wpl_block_if &copy) :
		wpl_block_conditional(copy),
		next_else_if(),
		flag(F_IF)
	{}
	wpl_block_if(const wpl_block_if &copy, int flag) :
		wpl_block_conditional(copy),
		next_else_if(),
		flag(flag)
	{}

	const char *get_name() const override {
		return name;
	}

	wpl_block_if *new_instance() const override {
		return new wpl_block_if(*this);
	}

	wpl_state *new_state (wpl_state *parent, wpl_namespace_session *nss, wpl_io *io) override;
	void parse_value(wpl_namespace *ns);
	int run(wpl_state *state, wpl_value *final_result) override;
};

