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

#include "block.h"
#include "operator_return_values.h"
#include "debug.h"

#include <cstddef>

class wpl_value;

class wpl_block_if : public wpl_block {
	private:
	wpl_block_if *next_else_if;

	public:
	wpl_block_if() {
		next_else_if = NULL;
	}

	~wpl_block_if() {
		if (next_else_if) {
			delete next_else_if;
		}
	}

	void set_next_else_if(wpl_block_if *next_else_if) {
		this->next_else_if = next_else_if;
	}

	int run(wpl_state *state, wpl_value *final_result) override;
};

