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

#include "../libwpl/value_string.h"
#include "../libwpl/operator.h"
#include "../libwpl/namespace.h"
#include "sql.h"

#include <string>
#include <memory>

using namespace std;

static const char *wpl_typename_sql = "{sql}";
static const int wpl_type_precedence_sql = wpl_type_precedence_middle;

class wpl_value_sql : public wpl_value {
	public:

	shared_ptr<wpl_sql> my_sql; // <-- my_sql==hahaha :-)

	const char *get_type_name() const override {
		return wpl_typename_sql;
	}
	int get_precedence() const override {
		return wpl_type_precedence_sql;
	}
	wpl_value_sql(wpl_sql *new_sql) :
		my_sql(new_sql)
	{}
	wpl_value_sql() {
	}

	wpl_value_sql *clone() const override {
		return new wpl_value_sql(*this);
	}
	wpl_value_sql *clone_empty() const override {
		return new wpl_value_sql();
	}
	wpl_sql *get_sql() {
		return my_sql.get();
	}
	shared_ptr<wpl_sql> &get_sql_shared_ptr() {
		return my_sql;
	}
	bool isStruct() override {
		return true;
	}
	bool set_strong(wpl_value *value) override {
		wpl_value_sql *src;
		if (!(src = dynamic_cast<wpl_value_sql*>(value))) {
			return false;
		}
		my_sql = src->get_sql_shared_ptr();
	}
};

