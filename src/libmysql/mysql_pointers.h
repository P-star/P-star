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

#ifdef WIN32
    #include <windows.h>
    #include <mysql.h>
#else
    #include <mysql/mysql.h>
#endif

#include <vector>

class wpl_mysql_stmt_holder {
	private:
	MYSQL_STMT *stmt;

	public:
	wpl_mysql_stmt_holder(MYSQL *mysql) {
		stmt = mysql_stmt_init(mysql);
	}
	~wpl_mysql_stmt_holder() {
		if (stmt) {
			mysql_stmt_close(stmt);
		}
	}
	MYSQL_STMT *get_stmt() {
		return stmt;
	}
};

class wpl_mysql_holder {
	private:
	MYSQL *mysql;

	public:
	wpl_mysql_holder() {
		if (!(mysql = mysql_init(NULL))) {
			throw runtime_error("Could not initialize MYSQL structure");
		}
		mysql_options(mysql, MYSQL_SET_CHARSET_NAME, "utf8");
	}
	~wpl_mysql_holder() {
		mysql_close(mysql);
	}
	MYSQL *get_mysql() {
		return mysql;
	}
};
