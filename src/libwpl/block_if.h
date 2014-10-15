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
	public:
	enum {
		F_IF,
		F_ELSE_IF,
		F_ELSE
	};

	const int flag;
	unique_ptr<wpl_block_if> next_else_if;

	public:
	wpl_block_if(const int flag) : flag(flag) {}
	wpl_state *new_state (wpl_namespace_session *nss, wpl_io *io) override;
	void parse_value(wpl_namespace *ns);
	int run(wpl_state *state, wpl_value *final_result) override;
};

