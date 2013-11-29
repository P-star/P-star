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
#include "mysql_types.h"
#include "../libwpl/namespace.h"
#include "../libwpl/types.h"

using namespace std;

class wpl_mysql : public wpl_namespace {
};

wpl_mysql mysql;

static wpl_sql sql;

#define REGISTER_TYPE(name)						\
	static wpl_type_##name mysql_type_constant_##name;		\
	mysql_type_global_##name = &mysql_type_constant_##name;			\
	mysql.new_register_parseable(&mysql_type_constant_##name);

void register_identifiers() {
	// This order is important!!
	REGISTER_TYPE(MYSQL_ROW)
	REGISTER_TYPE(MYSQL_STMT)
	REGISTER_TYPE(MYSQL)

	mysql.new_register_parseable(&sql);
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
