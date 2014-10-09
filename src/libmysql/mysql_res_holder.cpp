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

#include "mysql_res_holder.h"

#include "../libwpl/value_bool.h"
#include "../libwpl/value_int.h"
#include "../libwpl/value_longlongint.h"
#include "../libwpl/value_uint.h"
#include "../libwpl/value_longlonguint.h"
#include "../libwpl/value_float.h"
#include "../libwpl/value_double.h"
#include "../libwpl/value_vstring.h"

#include <cstring>

wpl_mysql_res_holder::wpl_mysql_res_holder() {
	memset(fields, 0, sizeof(fields));
	fields_count = 0;
	stmt = NULL;
	res = NULL;
}

wpl_mysql_res_holder::~wpl_mysql_res_holder() {
	if (res) {
		mysql_free_result(res);
	}
}
/*
   XXX
   Not thread-safe
   */
void wpl_mysql_res_holder::update(MYSQL_STMT *stmt) {
	if (res) {
//		mysql_free_result(res);
		throw runtime_error("mysql_res_holder::update() not cleaned up");
	}
	res = mysql_stmt_result_metadata(stmt);
	if (!res) {
		this->stmt = NULL;
		return;
	}

	this->stmt = stmt;

	fields_count = mysql_num_fields(res);
	if (fields_count > WPL_MYSQL_BIND_MAX) {
		throw runtime_error("MySQL returned too many result fields");
	}

	memset (bind, 0, sizeof(bind));

	int i = 0;
	MYSQL_FIELD *field;
	while (field = mysql_fetch_field (res)) {
		wpl_mysql_field &my_field = fields[i];

		my_field.name = field->name;
		bind[i].length = &my_field.length;
		bind[i].is_null = &my_field.is_null;
		bind[i].error = &my_field.error;

		int old_precedence = my_field.value.get()
				? my_field.value->get_precedence()
				: 0
		;

		switch (field->type) {
			case MYSQL_TYPE_TINY:
			case MYSQL_TYPE_SHORT:
			case MYSQL_TYPE_LONG:
			case MYSQL_TYPE_INT24:
			case MYSQL_TYPE_YEAR:
			case MYSQL_TYPE_NEWDECIMAL:
				if (	(field->flags & UNSIGNED_FLAG) &&
					 old_precedence != wpl_type_precedence_uint
				) {
					my_field.value.reset (new wpl_value_uint());
				}
				else if (old_precedence != wpl_type_precedence_int)
				{
					my_field.value.reset (new wpl_value_int());
				}
				bind[i].buffer_type = MYSQL_TYPE_LONG;
				bind[i].buffer = my_field.value->toVoid();
				break;
			case MYSQL_TYPE_LONGLONG:
				if (	(field->flags & UNSIGNED_FLAG) &&
					 old_precedence != wpl_type_precedence_lluint
				) {
					my_field.value.reset (new wpl_value_lluint());
				}
				else if (old_precedence != wpl_type_precedence_llint)
				{
					my_field.value.reset (new wpl_value_llint());
				}
				bind[i].buffer_type = MYSQL_TYPE_LONGLONG;
				bind[i].buffer = my_field.value->toVoid();
				break;
			case MYSQL_TYPE_FLOAT:
				if (old_precedence != wpl_type_precedence_float){
					my_field.value.reset (new wpl_value_float());
				}
				bind[i].buffer_type = MYSQL_TYPE_FLOAT;
				bind[i].buffer = my_field.value->toVoid();
				break;
			case MYSQL_TYPE_DOUBLE:
				if (old_precedence != wpl_type_precedence_double){
					my_field.value.reset (new wpl_value_double());
				}
				bind[i].buffer_type = MYSQL_TYPE_DOUBLE;
				bind[i].buffer = my_field.value->toVoid();
				break;
			case MYSQL_TYPE_BLOB:
			case MYSQL_TYPE_TIME:
			case MYSQL_TYPE_DATE:
			case MYSQL_TYPE_DATETIME:
				if (old_precedence != wpl_type_precedence_vstring){
					my_field.value.reset (new wpl_value_vstring(32));
				}
				bind[i].buffer_type = MYSQL_TYPE_STRING;
				bind[i].buffer_length = 0;
				bind[i].buffer = NULL;
				bind[i].length = &my_field.real_length;
				break;
			default:
				if (old_precedence != wpl_type_precedence_vstring){
					my_field.value.reset (new wpl_value_vstring(field->length));
				}
				bind[i].buffer_type = MYSQL_TYPE_STRING;
				bind[i].buffer_length = field->length;
				bind[i].buffer = my_field.value->toVoid();
		};

		i++;
	}

	if (mysql_stmt_bind_result(stmt, bind) != 0) {
		cerr << "MySQL error: " << mysql_stmt_error(stmt) << endl;
		throw runtime_error("Could not bind result");
	}
}

wpl_mysql_field *wpl_mysql_res_holder::get_field (int index) {
	return &fields[index];
}

wpl_mysql_field *wpl_mysql_res_holder::get_field (const string &name) {
	for (int i = 0; i < fields_count; i++) {
		wpl_mysql_field &field = fields[i];
		if (name == field.name) {
			return &field;
		}
	}
	return NULL;
}

int wpl_mysql_res_holder::num_fields() {
	return mysql_num_fields(res);
}

bool wpl_mysql_res_holder::fetch() {
	int ret = mysql_stmt_fetch(stmt);

	if (ret == 0) {
		return true;
	}
	else if (ret == MYSQL_DATA_TRUNCATED) {
		refetch_truncated();
		return true;
	}
	else if (ret == MYSQL_NO_DATA) {
		return false;
	}

	cerr << "MySQL error: " << mysql_stmt_error(stmt) << endl;
	throw runtime_error("MySQL fetch failed");
}

void wpl_mysql_res_holder::refetch_truncated() {
	int i = -1;
	for (wpl_mysql_field &field : fields) {
		i++;
		if (!field.error) {
			continue;
		}

		int new_length = field.real_length;

		field.value->resize(new_length);

		MYSQL_BIND my_bind;
		memset (&my_bind, 0, sizeof (my_bind));

		my_bind.buffer_type = bind[i].buffer_type;
		my_bind.buffer_length = new_length;
		my_bind.buffer = field.value->toVoid();

		if (mysql_stmt_fetch_column(stmt, &my_bind, i, 0)) {
			cerr << "MySQL error: " << mysql_stmt_error(stmt) << endl;
			throw runtime_error("Could not re-fetch column");
		}
	}
}
