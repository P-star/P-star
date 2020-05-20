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

#include "../libwpl/value.h"

#ifdef WIN32
    #include <windows.h>
    #include <mysql.h>
#else
    #include <mysql/mysql.h>
#endif

#include <memory>
#include <vector>

using namespace std;

struct wpl_mysql_field {
	const char *name;
	unique_ptr<wpl_value> value;
	long unsigned int real_length;
	long unsigned int length;
	bool is_null;
	bool error;
	MYSQL_FIELD *field;
};

class wpl_mysql_res_holder {
	private:
	MYSQL_RES *res;
	MYSQL_STMT *stmt;

	int fields_count;
	wpl_mysql_field fields[WPL_MYSQL_BIND_MAX];
	MYSQL_BIND bind[WPL_MYSQL_BIND_MAX];

	private:
	wpl_mysql_res_holder (const wpl_mysql_res_holder &copy) {};
	wpl_mysql_res_holder &operator= (const wpl_mysql_res_holder &rhs) {};

	public:
	wpl_mysql_res_holder();
	~wpl_mysql_res_holder();

	void update(MYSQL_STMT *stmt);
	wpl_mysql_field *get_field(int index);
	wpl_mysql_field *get_field(const string &name);
	int num_fields();
	bool fetch();
	void refetch_truncated();

	MYSQL_RES *get_res() {
		return res;
	}
};
