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

#include "scene.h"
#include "scene_state.h"

wpl_state *wpl_scene::new_state(wpl_state *parent, wpl_namespace_session *nss, wpl_io *io) {
	return new wpl_scene_state(parent, nss, io, this, base_scenes);
}

int wpl_scene::run (wpl_state *state, wpl_value *final_result) {
	wpl_scene_state *scene_state = (wpl_scene_state*) state;

	int index = 0;
	int ret;
	for (wpl_scene *base : base_scenes) {
		ret = scene_state->run_base_scene(base, index++, final_result);
	}
	return wpl_block::run(state, final_result);
}
	
wpl_type_complete *wpl_scene::find_complete_type(const char *name) const {
	for (auto it : base_scenes) {
		if (wpl_type_complete *ret = it->find_complete_type(name)) {
	 		return ret;
		}
	}
	return wpl_namespace::find_complete_type(name);
}
