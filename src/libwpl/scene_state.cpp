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

#include "scene.h"
#include "scene_state.h"

wpl_scene_state::wpl_scene_state (
		wpl_namespace_session *parent,
		wpl_namespace *template_namespace,
		list<wpl_scene*> base_scenes
		) :
	wpl_block_state (parent, template_namespace)
{
	for (wpl_scene *base : base_scenes) {
		base_states.emplace_back((wpl_scene_state*) base->new_state(parent));
	}
}

int wpl_scene_state::run_base_scene (wpl_scene *scene, int index, wpl_value *final_result) {
	return scene->run(base_states[index].get(), final_result);
}

wpl_variable *wpl_scene_state::find_variable (const char *name, int ctx) {
	wpl_variable *variable = wpl_namespace_session::find_variable (name, ctx);
	if (variable) {
		return variable;
	}
	for (unique_ptr<wpl_scene_state> &base_state : base_states) {
		if (variable = base_state->find_variable (name, WPL_NSS_CTX_CHILD)) {
			return variable;
		}
	}
	return NULL;
}

wpl_function *wpl_scene_state::find_function (const char *name, int ctx) {
	wpl_function *function = wpl_namespace_session::find_function (name, ctx);
	if (function) {
		return function;
	}
	for (unique_ptr<wpl_scene_state> &base_state : base_states) {
		if (function = base_state->find_function (name, WPL_NSS_CTX_CHILD)) {
			return function;
		}
	}
	return NULL;
}
