/*

-------------------------------------------------------------

Copyright (c) MMXIII-MMXIV Atle Solbakken
atle@goliathdns.no
Copyright (c) MMXIV Sebastian Baginski
sebthestampede@gmail.com

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
#include "../libwpl/value_time.h"
#include "../libwpl/value_struct.h"
#include "value_sql.h"
#include "../libwpl/function.h"
#include "parasites.h"

#include <cstring>

void wpl_mysql_bind (
		wpl_namespace_session *nss,
		MYSQL_STMT *stmt,
		wpl_sql *sql
) {
	const int param_count = mysql_stmt_param_count(stmt);

	if (param_count == 0)
		return;

	if (param_count > WPL_MYSQL_BIND_MAX) {
		throw runtime_error("MySQL error: Too many bind parameters");
	}

	mysql_stmt_reset(stmt);

	vector<wpl_value*> values;
	unique_ptr<wpl_text_state> text_state((wpl_text_state*) sql->new_state(NULL, nss, NULL));

	values.reserve(param_count);
	sql->get_params(text_state.get(), values);

	if (values.size() != param_count) {
		throw runtime_error("MySQL error: Bind parameter count mismatch");
	}

	MYSQL_BIND bind[param_count];
	memset (&bind, 0, sizeof(bind));

	int i = 0;
	for (wpl_value *value : values) {
		switch (value->get_precedence()) {
			case wpl_type_precedence_bool:
				/* XXX This might not be cross-platform */
				bind[i].buffer_type = MYSQL_TYPE_TINY;
				bind[i].buffer = (char*)value->toVoid();
				break;
			case wpl_type_precedence_int:
			case wpl_type_precedence_uint:
				bind[i].buffer_type = MYSQL_TYPE_LONG;
				bind[i].buffer = (char*) value->toVoid();
				break;
/* XXX Problems here with the size of long long values, differs in P* and MySQL
			case wpl_type_precedence_llint:
			case wpl_type_precedence_lluint:
				bind[i].buffer_type = MYSQL_TYPE_LONGLONG;
				bind[i].buffer = (char*) value->toVoid();
				break;*/
			case wpl_type_precedence_string:
				{
					wpl_value_string *v_string = (wpl_value_string*) value;
					wpl_mysql_string_parasite *parasite = v_string->find_parasite<wpl_mysql_string_parasite>((void*) stmt);
					if (!parasite) {
						parasite = new wpl_mysql_string_parasite(v_string, nss, stmt, sql);
						v_string->register_parasite(parasite);
					}
					bind[i].buffer_type = MYSQL_TYPE_STRING;
					bind[i].buffer = (char*) parasite->get();
					bind[i].buffer_length = parasite->get_size();
				}
				break;
			case wpl_type_precedence_float:
				bind[i].buffer_type = MYSQL_TYPE_FLOAT;
				bind[i].buffer = (char*)value->toVoid();
				break;
			case wpl_type_precedence_double:
				bind[i].buffer_type = MYSQL_TYPE_DOUBLE;
				bind[i].buffer = (char*)value->toVoid();
				break;
			case wpl_type_precedence_time:
				{
					wpl_value_time * v_time = (wpl_value_time*)value;
					wpl_mysql_time_parasite *parasite = v_time->find_parasite<wpl_mysql_time_parasite>((void*) stmt);
					if (!parasite) {
						parasite = new wpl_mysql_time_parasite(v_time, (void*) stmt);
						v_time->register_parasite(parasite);
						parasite->notify();
					}
					bind[i].buffer_type = MYSQL_TYPE_DATETIME;
					bind[i].buffer = parasite->get();
				}
				break;
			default:
				cerr << "MySQL error while binding value of type '" << value->get_type_name() << "'\n";
				throw runtime_error("Unsupported type for MySQL queries");
		}
		++i;
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
	wpl_function_state *function_state = (wpl_function_state*) state;

	wpl_value_MYSQL_STMT *this_stmt =
		function_state->find_variable_value<wpl_value_MYSQL_STMT>("prepare()", "this");
	wpl_value_sql *value_sql =
		function_state->find_variable_value<wpl_value_sql>("prepare()", "sql");

	wpl_sql *sql = value_sql->get_sql();

	string stmt_string;
	sql->get_stmt_string(stmt_string);

	bool ret = true;
	if (mysql_stmt_prepare(this_stmt->get_stmt(), stmt_string.c_str(), stmt_string.size()) != 0) {
		cerr << "MySQL prepare statement '" << stmt_string << "' failed\n";
		ret = false;
	}

	wpl_mysql_bind (function_state->get_parent(), this_stmt->get_stmt(), sql);

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
	wpl_function_state *function_state = (wpl_function_state*) state;

	wpl_value_MYSQL_STMT *this_stmt =
		function_state->find_variable_value<wpl_value_MYSQL_STMT>("execute()", "this");

	MYSQL_STMT *stmt = this_stmt->get_stmt();
	if (!stmt) {
		throw runtime_error("MySQL error: stmt_execute(): Statement object not initialized yet");
	}

	if (this_stmt->get_res()) {
		this_stmt->reset_res();
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
	wpl_function_state *function_state = (wpl_function_state*) state;

	wpl_value_MYSQL_STMT *this_stmt =
		function_state->find_variable_value<wpl_value_MYSQL_STMT>("error()", "this");

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
		wpl_function (
			wpl_mysql_get_global_type_MYSQL_ROW(),
			"get_row_iterator",
			WPL_VARIABLE_ACCESS_PUBLIC
		)
	{}
	int run (wpl_state *state, wpl_value *final_result);
};


int wpl_mysql_stmt_get_row_iterator::run (
		wpl_state *state,
		wpl_value *final_result
		)
{
	wpl_function_state *function_state = (wpl_function_state*) state;

	wpl_value_MYSQL_STMT *this_stmt =
		function_state->find_variable_value<wpl_value_MYSQL_STMT>("get_row_iterator()", "this");

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

class wpl_mysql_stmt_ctor : public wpl_function {
	public:
	wpl_mysql_stmt_ctor() :
		wpl_function (
			wpl_type_global_void,
			"MYSQL_STMT",
			WPL_VARIABLE_ACCESS_PUBLIC
		)
	{
		wpl_variable_holder arg("arg", new wpl_value_MYSQL_STMT(), WPL_VARIABLE_ACCESS_PRIVATE);
		register_identifier(&arg);
		generate_signature();
	}
	int run (wpl_state *state, wpl_value *final_result);
};


int wpl_mysql_stmt_ctor::run (
		wpl_state *state,
		wpl_value *final_result
		)
{
	wpl_function_state *function_state = (wpl_function_state*) state;

	wpl_value_MYSQL_STMT *this_stmt =
		function_state->find_variable_value<wpl_value_MYSQL_STMT>("ctor()", "this");
	wpl_value_MYSQL_STMT *argument =
		function_state->find_variable_value<wpl_value_MYSQL_STMT>("ctor()", "arg");

	this_stmt->set(argument);

	return (WPL_OP_NO_RETURN|WPL_OP_FUNCTION_DID_RUN);
}

wpl_type_MYSQL_STMT::wpl_type_MYSQL_STMT() :
	wpl_struct(wpl_typename_MYSQL_STMT, true)
{
	wpl_variable_holder this_var("this", new wpl_value_MYSQL_STMT(), WPL_VARIABLE_ACCESS_PRIVATE);
	register_identifier(&this_var);

	register_identifier(new wpl_mysql_stmt_prepare());
	register_identifier(new wpl_mysql_stmt_execute());
	register_identifier(new wpl_mysql_stmt_error());
	register_identifier(new wpl_mysql_stmt_get_row_iterator());
	register_identifier(new wpl_mysql_stmt_ctor());
}

bool wpl_value_MYSQL_STMT::set_strong (wpl_value *value) {
	wpl_value_MYSQL_STMT *src;
	wpl_value_struct *s;

	if (src = dynamic_cast<wpl_value_MYSQL_STMT*>(value)) {
		return true;
	}
	else if (s = dynamic_cast<wpl_value_struct*>(value)) {
		src = dynamic_cast<wpl_value_MYSQL_STMT*>(s->get_this()->get_value());
	}

	if (!src) {
		return false;
	}

	mysql_stmt = src->get_stmt_shared_ptr();
	mysql_res = src->get_res_shared_ptr();

	return true;
}
