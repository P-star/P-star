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

#include "./mysql.h"
#include "mysql_stmt.h"
#include "mysql_types.h"
#include "../libwpl/operator.h"
#include "../libwpl/value_string.h"
#include "../libwpl/value_bool.h"
#include "../libwpl/function.h"

#ifdef WIN32
#include <windows.h>
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#include <cstring>

class wpl_mysql_connect : public wpl_function {
	public:
	wpl_mysql_connect() :
		wpl_function(wpl_type_global_bool, "connect")
	{
		wpl_variable_holder host("host", new wpl_value_string(), WPL_VARIABLE_ACCESS_PRIVATE);
		wpl_variable_holder user("user", new wpl_value_string(), WPL_VARIABLE_ACCESS_PRIVATE);
		wpl_variable_holder passwd("passwd", new wpl_value_string(), WPL_VARIABLE_ACCESS_PRIVATE);
		register_identifier(&host);
		register_identifier(&user);
		register_identifier(&passwd);
	}
	int run (wpl_state *state, wpl_value *final_result);
};

int wpl_mysql_connect::run (wpl_state *state, wpl_value *final_result) {
	wpl_variable *this_var;
	wpl_variable *host_var;
	wpl_variable *user_var;
	wpl_variable *passwd_var;

	wpl_block_state *block_state = (wpl_block_state*) state;

	if (!(this_var = block_state->find_variable("this", WPL_NSS_CTX_SELF))) {
		throw runtime_error("wpl_mysql_connect(): Could not find 'this' variable");
	}
	if (!(host_var = block_state->find_variable("host", WPL_NSS_CTX_SELF))) {
		throw runtime_error("wpl_mysql_connect(): Could not find 'host' variable");
	}
	if (!(user_var = block_state->find_variable("user", WPL_NSS_CTX_SELF))) {
		throw runtime_error("wpl_mysql_connect(): Could not find 'user' variable");
	}
	if (!(passwd_var = block_state->find_variable("passwd", WPL_NSS_CTX_SELF))) {
		throw runtime_error("wpl_mysql_connect(): Could not find 'passwd' variable");
	}

	wpl_value_MYSQL *this_mysql = (wpl_value_MYSQL*) this_var->get_value();
	const char *host = host_var->get_value()->toString().c_str();
	const char *user = user_var->get_value()->toString().c_str();
	const char *passwd = passwd_var->get_value()->toString().c_str();

#ifdef WPL_DEBUG_MYSQL
	DBG("MYSQL (" << this << ") connect (" << host << ", " << user << ", " << passwd << ")\n");
#endif
	if (mysql_real_connect(this_mysql->get_mysql(), host, user, passwd, NULL, 0, NULL, 0) == NULL) {
		cerr << "Failed to connect to mysql database: Error: " <<
			mysql_error(this_mysql->get_mysql()) << endl;
		throw runtime_error("Database connection failed");
	}

	if (mysql_set_character_set(this_mysql->get_mysql(), "utf8")) {
		cerr << "MySQL error while setting character set: " << mysql_error(this_mysql->get_mysql()) << endl;
		throw runtime_error("Could not set character set");
	}

	((wpl_value_bool*) final_result)->set(true);

	return WPL_OP_OK;
}

class wpl_mysql_select_db : public wpl_function {
	public:
	wpl_mysql_select_db() :
		wpl_function(wpl_type_global_bool, "select_db")
	{
		wpl_variable_holder db("db", new wpl_value_string(), WPL_VARIABLE_ACCESS_PRIVATE);
		register_identifier(&db);
	}
	int run (wpl_state *state, wpl_value *final_result);
};

int wpl_mysql_select_db::run (wpl_state *state, wpl_value *final_result) {
	wpl_variable *this_var;
	wpl_variable *db_var;

	wpl_block_state *block_state = (wpl_block_state*) state;

	if (!(this_var = block_state->find_variable("this", WPL_NSS_CTX_SELF))) {
		throw runtime_error("wpl_mysql_select_db(): Could not find 'this' variable");
	}

	if (!(db_var = block_state->find_variable("db", WPL_NSS_CTX_SELF))) {
		throw runtime_error("wpl_mysql_select_db(): Could not find 'db' variable");
	}

	wpl_value_MYSQL *this_mysql = (wpl_value_MYSQL*) this_var->get_value();
	const char *db = db_var->get_value()->toString().c_str();

	if (!this_mysql->get_mysql()) {
		throw runtime_error("MySQL error: select_db() called with no active connection");
	}

	((wpl_value_bool*) final_result)->set(mysql_select_db (this_mysql->get_mysql(), db) == 0);

	return WPL_OP_OK;
}

class wpl_mysql_error : public wpl_function {
	public:
	wpl_mysql_error() :
		wpl_function(wpl_type_global_string, "error")
	{}
	int run (wpl_state *state, wpl_value *final_result);
};

int wpl_mysql_error::run (wpl_state *state, wpl_value *final_result) {
	wpl_variable *this_var;

	wpl_block_state *block_state = (wpl_block_state*) state;

	if (!(this_var = block_state->find_variable("this", WPL_NSS_CTX_SELF))) {
		throw runtime_error("wpl_mysql_error(): Could not find 'this' variable");
	}

	wpl_value_MYSQL *this_mysql = (wpl_value_MYSQL*) this_var->get_value();

	((wpl_value_string*) final_result)->set(mysql_error (this_mysql->get_mysql()));

	return WPL_OP_OK;
}

class wpl_mysql_stmt_init : public wpl_function {
	public:
	wpl_mysql_stmt_init() :
		wpl_function(mysql_type_global_MYSQL_STMT, "new_statement")
	{}
	int run (wpl_state *state, wpl_value *final_result);
};

int wpl_mysql_stmt_init::run (wpl_state *state, wpl_value *final_result) {
	wpl_variable *this_var;

	wpl_block_state *block_state = (wpl_block_state*) state;

	if (!(this_var = block_state->find_variable("this", WPL_NSS_CTX_SELF))) {
		throw runtime_error("wpl_stmt_init(): Could not find 'this' variable");
	}
	wpl_value_MYSQL *this_mysql = (wpl_value_MYSQL*) this_var->get_value();

	MYSQL *mysql;
	if (!(mysql = this_mysql->get_mysql())) {
		throw runtime_error("MySQL error: statement init() called with no active connection");
	}

	wpl_value_struct *stmt_struct = (wpl_value_struct*) final_result;
	wpl_value_MYSQL_STMT *stmt = ((wpl_value_MYSQL_STMT*) stmt_struct->get_this()->get_value());

	stmt->init(mysql);

	return WPL_OP_OK;
}

wpl_type_MYSQL::wpl_type_MYSQL() :
	wpl_struct(wpl_typename_MYSQL)
{
	wpl_variable_holder this_var("this", new wpl_value_MYSQL(), WPL_VARIABLE_ACCESS_PRIVATE);
	register_identifier(&this_var);
	register_identifier(new wpl_mysql_connect());
	register_identifier(new wpl_mysql_select_db());
	register_identifier(new wpl_mysql_error());
	register_identifier(new wpl_mysql_stmt_init());
}
