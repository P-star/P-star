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

#include "block_state.h"

#include <memory>
#include <vector>

class wpl_scene;

class wpl_scene_state : public wpl_block_state {
	private:
	vector<unique_ptr<wpl_scene_state>> base_states;

	public:
	wpl_scene_state (
			wpl_namespace_session *parent,
			wpl_namespace *template_namespace,
			list<wpl_scene*> base_scenes
	);

	wpl_variable *find_variable(const char *name, int ctx) override;
	wpl_function *find_function(const char *name, int ctx) override;

	int run_base_scene (wpl_scene *scene, int index, wpl_value *final_result);
};
