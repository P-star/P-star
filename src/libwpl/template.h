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

#include "text.h"
#include "identifier.h"
#include "value.h"

#include "global.h"

#include <set>

extern const wpl_global_template *global_template;

class wpl_template : public wpl_text {
	private:
	struct id_var_pair {
		wpl_value *id;
		wpl_value *value;
	};

	public:
	wpl_template (const char *name) : wpl_text(name, global_template) {}
	virtual ~wpl_template() {}
	virtual void suicide() override {
		delete this;
	}
};
