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

#include <iostream>
#include <stdexcept>
#include <mutex>

#ifdef WIN32
    #include <windows.h>
    #include <mysql.h>
#else
    #include <mysql/mysql.h>
    #include "module.h"
#endif /* WIN32 */

#include "libwpl_mysql.h"
#include "./mysql.h"
#include "mysql_stmt.h"
#include "mysql_row.h"
#include "mysql_types.h"
#include "../libwpl/namespace.h"
#include "../libwpl/types.h"
#include "../libwpl/global.h"

using namespace std;

// Types for this module
static wpl_sql sql;
static wpl_type_MYSQL mysql_type_constant_MYSQL;
static wpl_type_MYSQL_STMT mysql_type_constant_MYSQL_STMT;
static wpl_type_MYSQL_ROW mysql_type_constant_MYSQL_ROW;

class wpl_global_mysql : public wpl_global {
	public:
	wpl_global_mysql() : wpl_global()
	{
		// Nothing fancy in SQL-block namespace (yet)
	}
};

const wpl_global_mysql constant_global_mysql;
const wpl_namespace *global_mysql_sql = &constant_global_mysql;

#define REGISTER_TYPE(name)						\
	register_parseable(&mysql_type_constant_##name);	\

static bool is_initialized = false;
static mutex is_initialized_lock;

class wpl_mysql : public wpl_namespace {
	public:
	wpl_mysql() {
		// This order is important!!
		REGISTER_TYPE(MYSQL_ROW)
		REGISTER_TYPE(MYSQL_STMT)
		REGISTER_TYPE(MYSQL)

		register_parse_and_run(&sql);
	}
	~wpl_mysql() {
	}
};

wpl_mysql mysql;

wpl_type_complete *wpl_mysql_get_global_type_MYSQL_STMT() {
	return &mysql_type_constant_MYSQL_STMT;
}

wpl_type_complete *wpl_mysql_get_global_type_MYSQL_ROW() {
	return &mysql_type_constant_MYSQL_ROW;
}

void register_identifiers() {
}

#ifdef WIN32
void wpl_mysql_init() {
	lock_guard<mutex> lock(is_initialized_lock);

	if (is_initialized)
		return;

	if (mysql_library_init(0, NULL, NULL)) {
		throw runtime_error("Error while initializing the MySQL library\n");
	}

	is_initialized = true;
}

wpl_namespace *wpl_mysql_get_namespace() {
	return &mysql;
}

#else
int wpl_module_init(int argc, char **argv) {
	return 0;
}
void wpl_module_exit () {
	lock_guard<mutex> lock(is_initialized_lock);

	if (!is_initialized)
		return;

	mysql_library_end();

	is_initialized = false;
}
wpl_namespace *wpl_module_get_namespace() {
	return &mysql;
}
#endif
