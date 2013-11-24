/*

-------------------------------------------------------------

Copyright (c) MMIII Atle Solbakken
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

#include "../libwpl/text.h"
#include "../libwpl/namespace_session.h"

#include <vector>

class wpl_namespace;
class wpl_text_state;

class wpl_sql : public wpl_text {
	private:
	public:
	wpl_sql() : wpl_text("SQL") {
	}
	wpl_sql(const char *name) : wpl_text(name) {
	}
	wpl_sql *clone() const {
		return new wpl_sql();
	}
	void suicide() override {
		delete this;
	}
	void get_stmt_string(string &result);
	void get_params(wpl_text_state *text_state, vector<wpl_value*> &params);
	void parse_value(wpl_namespace *parent_namespace);
	void __parse_value(wpl_namespace *parent_namespace);
};
