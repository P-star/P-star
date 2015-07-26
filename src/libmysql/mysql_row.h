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
#include "sql.h"
#include "../libwpl/types.h"
#include "mysql_types.h"
#include "mysql_pointers.h"
#include "mysql_res_holder.h"

#ifdef WIN32
#include <windows.h>
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#include <memory>

class wpl_value_MYSQL_ROW : public wpl_value {
	private:

	shared_ptr<wpl_mysql_stmt_holder> mysql_stmt;
	shared_ptr<wpl_mysql_res_holder> mysql_res;

	public:
	wpl_value_MYSQL_ROW ()
	{}
	void init (
			shared_ptr<wpl_mysql_stmt_holder> &stmt_holder,
			shared_ptr<wpl_mysql_res_holder> &res_holder
	);
	const char *get_type_name() const { return wpl_typename_MYSQL_ROW; };
	int get_precedence() const { return wpl_type_precedence_MYSQL_ROW; };

	wpl_value_MYSQL_ROW *clone() const {
		return new wpl_value_MYSQL_ROW(*this);
	};

	wpl_value_MYSQL_ROW *clone_empty() const {
		return new wpl_value_MYSQL_ROW();
	};

	shared_ptr<wpl_mysql_res_holder> &get_res_shared_ptr() {
		return mysql_res;
	}

	shared_ptr<wpl_mysql_stmt_holder> &get_stmt_shared_ptr() {
		return mysql_stmt;
	}

	MYSQL_RES *get_res() {
		if (!(mysql_res.get())) {
			return NULL;
		}
		return mysql_res->get_res();
	}

	MYSQL_STMT *get_stmt() {
		if (!(mysql_stmt.get())) {
			return NULL;
		}
		return mysql_stmt->get_stmt();
	}

	bool set_strong (wpl_value *value) override;

	wpl_value *get_column(const string &name);
	wpl_value *get_column(int index);
	bool next_row ();

	virtual int do_operator (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const struct wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
	);

	bool isStruct() override {
		return true;
	}

	string toString() const override {
		return string(wpl_typename_MYSQL_ROW);
	};
};

class wpl_type_MYSQL_ROW : public wpl_type_complete {
	public:
	wpl_type_MYSQL_ROW() :
		wpl_type_complete(wpl_typename_MYSQL_ROW)
	{}
	wpl_value *new_instance() const override {
		wpl_value *ret = new wpl_value_MYSQL_ROW();
		return ret;
	}
	void suicide() {
		delete this;
	}
	int get_precedence() const {
		return wpl_type_precedence_MYSQL_ROW;
	}
};
