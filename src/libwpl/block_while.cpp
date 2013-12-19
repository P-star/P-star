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

#include "block_while.h"
#include "expression.h"

int wpl_block_while::run(wpl_state *state, wpl_value *final_result) {
	int ret = WPL_OP_NO_RETURN;
	wpl_block_state *block_state = (wpl_block_state*) state;

	while (check_run(block_state)) {
		ret = wpl_block::run(block_state, final_result);
		block_state->clear_child_states();

		if (ret & (WPL_OP_BREAK|WPL_OP_RETURN)) {
			ret &= ~(WPL_OP_BREAK);
			break;
		}
	}

	return ret;
}
