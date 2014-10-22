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

#pragma once

#include "identifier.h"
#include "block.h"

#include <list>

class wpl_scene : public wpl_block, public wpl_identifier {
	private:
	list<wpl_scene*> base_scenes;
	
	public:
	wpl_scene (const char *name) : wpl_identifier(name) {}
	virtual ~wpl_scene() {}

	wpl_state *new_state(wpl_state *parent, wpl_namespace_session *nss, wpl_io *io) override;
	int run (wpl_state *state, wpl_value *final_result) override;
	wpl_type_complete *find_complete_type(const char *name) const override;

	void add_base (wpl_scene *base) {
		base_scenes.push_back(base);
	}

	bool isScene() const {
		return true;
	}
};
