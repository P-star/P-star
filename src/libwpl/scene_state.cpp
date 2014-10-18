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
#include "value_unresolved.h"
#include "value_function_ptr.h"

wpl_scene_state::wpl_scene_state (
		wpl_state *parent,
		wpl_namespace_session *nss,
		wpl_io *io,
		wpl_namespace *template_namespace,
		list<wpl_scene*> base_scenes
		) :
	wpl_block_state (parent, nss, io, template_namespace)
{
	for (wpl_scene *base : base_scenes) {
		base_states.emplace_back((wpl_scene_state*) base->new_state(parent, nss, io));
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

int wpl_scene_state::do_operator_on_unresolved (
	wpl_value_unresolved_identifier *unresolved,
	wpl_expression_state *exp_state,
	wpl_value *final_result,
	int ctx
) {
	wpl_function *function = find_function_no_parent(unresolved->get().c_str(), ctx);

	if (!function) {
		int ret = WPL_OP_NAME_UNRESOLVED;
		for (unique_ptr<wpl_scene_state> &base_state : base_states) {
			ret = base_state->do_operator_on_unresolved(
				unresolved,
				exp_state,
				final_result,
				WPL_NSS_CTX_CHILD
			);
			if (!(ret & WPL_OP_NAME_UNRESOLVED)) {
				break;
			}
		}
		return ret;
	}

	wpl_value_function_ptr function_ptr(function, this, exp_state);
	return function_ptr.do_operator_recursive(exp_state, final_result);
}
