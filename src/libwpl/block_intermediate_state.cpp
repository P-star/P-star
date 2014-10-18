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

#include "block_intermediate_state.h"
#include "runable.h"

int wpl_block_intermediate_state::run_block (wpl_value *final_result) {
	if (!block_state.get()) {
		block_state.reset(block->new_state(this, this, &get_io()));
	}
	return block->run(block_state.get(), final_result);
}
