/*

-------------------------------------------------------------

Copyright (c) MMXIII-MMXIX Atle Solbakken
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

#include "state.h"
#include "block_state.h"

#include <memory>
#include <vector>

class wpl_value;
class wpl_runable;
class wpl_template;

class wpl_pragma_state : public wpl_state {
	protected:
	vector<unique_ptr<wpl_state>> child_state;

	public:
	wpl_pragma_state (wpl_state *parent, wpl_namespace_session *nss, wpl_io *io) :
		wpl_state(parent, nss, io)
	{
		child_state.resize(2);
	}

	wpl_template *find_template(const char *name) {
		return get_nss()->find_template(name);
	}
	wpl_scene *find_scene(const char *name) {
		return get_nss()->find_scene(name);
	}
	wpl_variable *find_variable(const char *name, int ctx) {
		return get_nss()->find_variable(name, ctx);
	}
	void set_children_count(int count) {
		child_state.resize(count);
	}
	int run_child (wpl_runable *child, int index, wpl_value *final_result);
	wpl_state *get_child_state(wpl_runable *child, int index);	
};
