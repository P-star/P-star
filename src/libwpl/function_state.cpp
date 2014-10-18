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

#include "function_state.h"
#include "value.h"
#include "block.h"

int wpl_function_state::run_block (wpl_block *block, wpl_value *final_result) {
	if (block_state.get() == nullptr) {
		block_state.reset (new wpl_block_state(this, this, &get_io(), this, block));
	}
	return block->run(block_state.get(), return_value.get());
}
