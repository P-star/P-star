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

#include <iostream>
#include <stdexcept>

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
#include "../libwpl/namespace.h"
#include "../libwpl/types.h"

using namespace std;

class wpl_mysql : public wpl_namespace {
};

wpl_mysql mysql;

#ifndef WIN32
wpl_type_complete *wpl_type_global_bool = NULL;
wpl_type_complete *wpl_type_global_string = NULL;
#endif

wpl_type_complete *wpl_type_global_MYSQL_ROW = NULL;
wpl_type_complete *wpl_type_global_MYSQL_STMT = NULL;
wpl_type_complete *wpl_type_global_MYSQL = NULL;

#define REGISTER_TYPE(name)					\
{								\
	wpl_type_##name *type =					\
		new wpl_type_##name(wpl_typename_##name);	\
	wpl_type_global_##name = type;				\
	mysql.register_identifier (type);			\
}


void register_identifiers() {
	// This order is important!!
#ifndef WIN32
	REGISTER_TYPE(bool)
	REGISTER_TYPE(string)
#endif

	REGISTER_TYPE(MYSQL_ROW)
	REGISTER_TYPE(MYSQL_STMT)
	REGISTER_TYPE(MYSQL)
	wpl_parseable *sql = new wpl_sql();
	mysql.register_identifier(sql);
	/*
	REGISTER_TYPE(MYSQL)

	mysql.register_identifier (new wpl_sql());*/
}

#ifdef WIN32
void wpl_mysql_init() {
	register_identifiers();
}

wpl_namespace *wpl_mysql_get_namespace() {
	return &mysql;
}

#else
int wpl_module_init(int argc, char **argv) {
	mysql_library_init(argc, argv, NULL);

	if (mysql_library_init(argc, argv, NULL)) {
		throw runtime_error("Error while initializing the MySQL library\n");
	}

	register_identifiers();

	return 0;
}

void wpl_module_exit () {
	mysql_library_end();
}

wpl_namespace *wpl_module_get_namespace() {
	return &mysql;
}
#endif