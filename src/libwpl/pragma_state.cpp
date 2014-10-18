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

#include "pragma_state.h"
#include "runable.h"

int wpl_pragma_state::run_child (wpl_runable *child, int index, wpl_value *final_result){
	if (child_state[index].get() == nullptr) {
		child_state[index].reset(child->new_state(this, get_nss(), &get_io()));
	}
	return child->run(child_state[index].get(), final_result);
}

wpl_state *wpl_pragma_state::get_child_state(wpl_runable *child, int index) {
	if (child_state[index].get() == nullptr) {
		child_state[index].reset(child->new_state(this, get_nss(), &get_io()));
	}
	return child_state[index].get();
}
