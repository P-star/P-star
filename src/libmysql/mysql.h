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
#include "../libwpl/value_struct.h"
#include "../libwpl/internal_function.h"
#include "mysql_pointers.h"
#include "mysql_types.h"

#include <iostream>
#include <memory>

using namespace std;

class wpl_type_MYSQL : public wpl_struct {
	public:
	wpl_type_MYSQL();
	void suicide() override {
		delete this;
	}
	int get_precedence() const {
		return wpl_type_precedence_MYSQL;
	}
};

class wpl_value_MYSQL : public wpl_value {
	private:
	shared_ptr<wpl_mysql_holder> mysql;

	public:
	wpl_value_MYSQL() {
		/*
		   DO NOT initialize wpl_mysql_holder here
		   */
	}

	const char *get_type_name() const override { return wpl_typename_MYSQL; };
	int get_precedence() const override { return wpl_type_precedence_MYSQL; };

	void suicide() override {
		delete this;
	}

	wpl_value_MYSQL *clone() const override {
		return new wpl_value_MYSQL(*this);
	}

	wpl_value_MYSQL *clone_empty() const override {
		return new wpl_value_MYSQL();
	}

	MYSQL *get_mysql() {
		if (mysql.get() == nullptr) {
			mysql.reset(new wpl_mysql_holder());
		}
		return mysql.get()->get_mysql();
	}

	shared_ptr<wpl_mysql_holder> &get_mysql_shared_ptr() {
		if (mysql.get() == nullptr) {
			mysql.reset(new wpl_mysql_holder());
		}
		return mysql;
	}

	bool set_strong (wpl_value *value) override {
		wpl_value_MYSQL *src;
		if (!(src = dynamic_cast<wpl_value_MYSQL*>(value))) {
			return false;
		}
		mysql = src->get_mysql_shared_ptr();
		return true;
	}

	void set_weak (wpl_value *value) override {
		if (!set_strong(value)) {
			ostringstream msg;
			msg << "Could not set MYSQL value to value of type " << value->get_type_name();
			throw new runtime_error(msg.str());
		}
	}

	string toString() const override {
		return string(wpl_typename_MYSQL);
	};
};
