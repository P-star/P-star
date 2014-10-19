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

#pragma once

#include "sql.h"
#include "../libwpl/value.h"
#include "../libwpl/struct.h"
#include "mysql_types.h"
#include "mysql_pointers.h"
#include "mysql_res_holder.h"

#include <memory>

void wpl_mysql_bind (
		wpl_namespace_session *nss,
		MYSQL_STMT *stmt,
		wpl_sql *sql
);

class wpl_type_MYSQL_STMT : public wpl_struct {
	public:
	wpl_type_MYSQL_STMT();
	void suicide() override {
		delete this;
	}
	int get_precedence() const {
		return wpl_type_precedence_MYSQL_STMT;
	}
};

class wpl_value_MYSQL_STMT : public wpl_value {
	private:

	shared_ptr<wpl_mysql_stmt_holder> mysql_stmt;
	shared_ptr<wpl_mysql_res_holder> mysql_res;

	public:
	void init (MYSQL *mysql) {
		mysql_stmt.reset(new wpl_mysql_stmt_holder(mysql));
		if (!(mysql_stmt->get_stmt())) {
			cerr << "Could not initialize MySQL statement: " << mysql_error(mysql) << endl;
			throw runtime_error("Error while initializing MySQL statement");
		}
		mysql_res.reset(new wpl_mysql_res_holder());
	}

	const char *get_type_name() const override { return wpl_typename_MYSQL_STMT; };
	int get_precedence() const override { return wpl_type_precedence_MYSQL_STMT; };

	wpl_value_MYSQL_STMT *clone() const override {
		return new wpl_value_MYSQL_STMT(*this);
	};

	wpl_value_MYSQL_STMT *clone_empty() const override {
		return new wpl_value_MYSQL_STMT();
	};

	shared_ptr<wpl_mysql_res_holder> &get_res_shared_ptr() {
		return mysql_res;
	}

	shared_ptr<wpl_mysql_stmt_holder> &get_stmt_shared_ptr() {
		return mysql_stmt;
	}

	void reset_res () {
		mysql_res.reset(new wpl_mysql_res_holder());
	}

	void update_res () {
		mysql_res->update(get_stmt());
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

	void set (wpl_value_MYSQL_STMT *src) {
		mysql_stmt = src->get_stmt_shared_ptr();
		mysql_res = src->get_res_shared_ptr();
	}

	bool set_strong (wpl_value *value) override;

	void set_weak (wpl_value *src) override {
		if (!set_strong(src)) {
			ostringstream msg;
			msg << "Could not set MYSQL_STMT to value of type '" << src->get_type_name() << "'\n";
			throw runtime_error(msg.str());
		}
	}

	string toString() const override {
		return string(wpl_typename_MYSQL_STMT);
	}
};
