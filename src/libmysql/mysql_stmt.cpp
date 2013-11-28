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

#include "mysql_stmt.h"
#include "mysql_row.h"
#include "mysql_types.h"
#include "sql.h"
#include "../libwpl/type_precedence.h"
#include "../libwpl/value_bool.h"
#include "value_sql.h"
#include "../libwpl/function.h"

#include <cstring>

void wpl_mysql_bind (
		wpl_namespace_session *nss,
		MYSQL_STMT *stmt,
		wpl_sql *sql
) {
	int param_count = mysql_stmt_param_count(stmt);

	if (param_count > WPL_MYSQL_BIND_MAX) {
		throw runtime_error("MySQL error: Too many bind parameters");
	}

	vector<wpl_value*> values;
	unique_ptr<wpl_text_state> text_state((wpl_text_state*) sql->new_state(nss));

	values.reserve(param_count);
	sql->get_params(text_state.get(), values);

	if (values.size() != param_count) {
		throw runtime_error("MySQL error: Bind parameter count mismatch");
	}

	MYSQL_BIND bind[WPL_MYSQL_BIND_MAX];
	memset (&bind, 0, sizeof(bind));

	int i = 0;
	for (wpl_value *value : values) {
		switch (value->get_precedence()) {
			case wpl_type_precedence_int:
				bind[i].buffer_type = MYSQL_TYPE_LONG;
				bind[i].buffer = (char*) value->toVoid();
				break;
			default:
				cerr << "MySQL error while binding value of type '" << value->get_type_name() << "'\n";
				throw runtime_error("Unsupported type for MySQL queries");
		}
		i++;
	}

	if (mysql_stmt_bind_param(stmt, bind) != 0) {
		cerr << "MySQL sais: " << mysql_stmt_error(stmt) << endl;
		throw runtime_error("MySQL error: Bind parameters failed");
	}
}

class wpl_mysql_stmt_prepare : public wpl_function {
	public:
	wpl_mysql_stmt_prepare() :
		wpl_function(wpl_type_global_bool, "prepare", WPL_VARIABLE_ACCESS_PUBLIC)
	{
		wpl_variable_holder sql("sql", new wpl_value_sql(), WPL_VARIABLE_ACCESS_PRIVATE);
		register_identifier(&sql);
	}
	int run (wpl_state *state, wpl_value *final_result);
};

int wpl_mysql_stmt_prepare::run (
		wpl_state *state,
		wpl_value *final_result
		)
{
	wpl_variable *this_var;
	wpl_variable *sql_var;

	wpl_block_state *block_state = (wpl_block_state*) state;

	if (!(this_var = block_state->find_variable("this", WPL_NSS_CTX_SELF))) {
		throw runtime_error("MySQL error: stmt_prepare(): Could not find 'this' variable");
	}
	if (!(sql_var = block_state->find_variable("sql", WPL_NSS_CTX_SELF))) {
		throw runtime_error("MySQL error: stmt_prepare(): Could not find 'sql' variable");
	}

	wpl_value_MYSQL_STMT *this_stmt = (wpl_value_MYSQL_STMT*) this_var->get_value();

	if (this_stmt->get_res()) {
		throw runtime_error("MySQL error: stmt_prepare() called before last result was freed");
	}

	wpl_value *value = sql_var->get_value()->dereference();
	wpl_value_sql *value_sql;
	if (!(value_sql = dynamic_cast<wpl_value_sql*>(value))) {
		cerr << "While processing argument of type " << value_sql->get_type_name() << ":\n";
		throw runtime_error("MySQL error: Invalid arguments for stmt_prepare()");
	}

	wpl_sql *sql = value_sql->get_sql();

	string stmt_string;
	sql->get_stmt_string(stmt_string);

	bool ret = true;
	if (mysql_stmt_prepare(this_stmt->get_stmt(), stmt_string.c_str(), stmt_string.size()) != 0) {
		cerr << "MySQL prepare statement '" << stmt_string << "' failed\n";
		ret = false;
	}

	wpl_mysql_bind (block_state, this_stmt->get_stmt(), sql);

	((wpl_value_bool*) final_result)->set(ret);

	return WPL_OP_OK;
}

class wpl_mysql_stmt_execute : public wpl_function {
	public:
	wpl_mysql_stmt_execute() :
		wpl_function(wpl_type_global_bool, "execute", WPL_VARIABLE_ACCESS_PUBLIC)
	{}
	int run (wpl_state *state, wpl_value *final_result);
};

int wpl_mysql_stmt_execute::run (
		wpl_state *state,
		wpl_value *final_result
		)
{
	wpl_variable *this_var;

	wpl_block_state *block_state = (wpl_block_state*) state;

	if (!(this_var = block_state->find_variable("this", WPL_NSS_CTX_SELF))) {
		throw runtime_error("MySQL error: stmt_execute(): Could not find 'this' variable");
	}

	wpl_value_MYSQL_STMT *this_stmt = (wpl_value_MYSQL_STMT*) this_var->get_value();

	MYSQL_STMT *stmt = this_stmt->get_stmt();
	if (!stmt) {
		throw runtime_error("MySQL error: stmt_execute(): Statement object not initialized yet");
	}

	bool ret = true;
	if (mysql_stmt_execute(stmt) == 0) {
		this_stmt->update_res();
		if (!(this_stmt->get_res())) {
			// No result
		}
	}
	else {
		ret = false;
	}

	((wpl_value_bool*) final_result)->set(ret);

	return WPL_OP_OK;
}

class wpl_mysql_stmt_error : public wpl_function {
	public:
	wpl_mysql_stmt_error() :
		wpl_function(wpl_type_global_string, "error", WPL_VARIABLE_ACCESS_PUBLIC)
	{}
	int run (wpl_state *state, wpl_value *final_result);
};

int wpl_mysql_stmt_error::run (
		wpl_state *state,
		wpl_value *final_result
		)
{
	wpl_variable *this_var;

	wpl_block_state *block_state = (wpl_block_state*) state;

	if (!(this_var = block_state->find_variable("this", WPL_NSS_CTX_SELF))) {
		throw runtime_error("MySQL error: stmt_execute(): Could not find 'this' variable");
	}

	wpl_value_MYSQL_STMT *this_stmt = (wpl_value_MYSQL_STMT*) this_var->get_value();

	MYSQL_STMT *stmt = this_stmt->get_stmt();
	if (!stmt) {
		throw runtime_error("MySQL error: stmt_error(): Statement object not initialized yet");
	}

	((wpl_value_string*) final_result)->set(mysql_stmt_error (stmt));

	return WPL_OP_OK;
}

class wpl_mysql_stmt_get_row_iterator : public wpl_function {
	public:
	wpl_mysql_stmt_get_row_iterator() :
		wpl_function(mysql_type_global_MYSQL_ROW, "get_row_iterator", WPL_VARIABLE_ACCESS_PUBLIC)
	{}
	int run (wpl_state *state, wpl_value *final_result);
};


int wpl_mysql_stmt_get_row_iterator::run (
		wpl_state *state,
		wpl_value *final_result
		)
{
	wpl_variable *this_var;

	wpl_block_state *block_state = (wpl_block_state*) state;

	if (!(this_var = block_state->find_variable("this", WPL_NSS_CTX_SELF))) {
		throw runtime_error("MySQL error: stmt_execute(): Could not find 'this' variable");
	}

	wpl_value_MYSQL_STMT *this_stmt = (wpl_value_MYSQL_STMT*) this_var->get_value();

	MYSQL_STMT *stmt = this_stmt->get_stmt();
	if (!stmt) {
		throw runtime_error("MySQL error: stmt_get_row_iterator(): Statement object not initialized yet");
	}

	MYSQL_RES *res;
	if (!(res = this_stmt->get_res())) {
		throw runtime_error("MySQL error: stmt_get_row_iterator() no result in prepared statement");
	}

	((wpl_value_MYSQL_ROW*) final_result)->init (
		this_stmt->get_stmt_shared_ptr(),
		this_stmt->get_res_shared_ptr()
	);

	return WPL_OP_OK;
}

wpl_type_MYSQL_STMT::wpl_type_MYSQL_STMT() :
	wpl_struct(wpl_typename_MYSQL_STMT)
{
	wpl_variable_holder this_var("this", new wpl_value_MYSQL_STMT(), WPL_VARIABLE_ACCESS_PRIVATE);
	register_identifier(&this_var);

	register_identifier(new wpl_mysql_stmt_prepare());
	register_identifier(new wpl_mysql_stmt_execute());
	register_identifier(new wpl_mysql_stmt_error());
	register_identifier(new wpl_mysql_stmt_get_row_iterator());
}
