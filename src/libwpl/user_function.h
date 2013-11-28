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

#include "block.h"
#include "matcher.h"
#include "function.h"

#include <sstream>

class wpl_block;
class wpl_function_state;
class wpl_namespace;
class wpl_type_complete;

class wpl_user_function : public wpl_function, public wpl_matcher {
	private:
	wpl_block block;

	public:
	wpl_user_function (
			const wpl_type_complete *return_type,
			const char *name,
			int access_flags
			);

	int run(wpl_state *state, wpl_value *final_result);
	void parse_value (wpl_namespace *ns);

};
