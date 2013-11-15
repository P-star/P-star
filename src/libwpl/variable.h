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

#pragma once

#include "identifier.h"
#include "cloneable.h"
#include "exception.h"
#include "debug.h"
#include "operator_return_values.h"

#include <memory>

using namespace std;

class wpl_value;

class wpl_variable : public wpl_identifier {
	protected:
	wpl_value *value;
	bool is_static;
	bool is_protected;

	public:
	wpl_variable (wpl_value *new_value);
	wpl_variable (wpl_value *new_value, const char *name);

	virtual ~wpl_variable () {};

	virtual wpl_variable *clone() const = 0;
	virtual wpl_value *clone_value() const;

	void setStatic() {
		is_static = true;
	}
	void setProtected() {
		is_protected = true;
	}

	bool isStatic() const {
		return is_static;
	}

	bool isVariable() const override {
		return true;
	}

	virtual void set_value (wpl_value *new_value) = 0;

	const char *get_type_name() const;
	wpl_value *get_value();
	void set_weak (wpl_value *value);
	bool set_strong (wpl_value *value);
};

class wpl_variable_pointer : public wpl_variable {
	public:
	wpl_variable_pointer (wpl_value *new_value) :
		wpl_variable (new_value)
	{}
	wpl_variable_pointer (wpl_value *new_value, const char *name) :
		wpl_variable (new_value, name)
	{}
	wpl_variable_pointer *clone() const;
	void set_value (wpl_value *new_value);
};

class wpl_variable_holder : public wpl_variable {
	public:
	wpl_variable_holder (const wpl_variable_holder &copy);
	wpl_variable_holder (wpl_value *new_value) :
		wpl_variable (new_value)
	{}
	wpl_variable_holder (wpl_value *new_value, const char *name) :
		wpl_variable (new_value, name)
	{}
	wpl_variable_holder (const char *name, wpl_value *new_value) :
		wpl_variable (new_value, name)
	{}
	~wpl_variable_holder();
	wpl_variable_holder *clone() const;
	void set_value (wpl_value *new_value);
};
