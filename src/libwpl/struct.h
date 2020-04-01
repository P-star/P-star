/*

-------------------------------------------------------------

Copyright (c) MMXIII-MMXIX Atle Solbakken
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

#include "type_precedence.h"
#include "typenames.h"
#include "type_user_incomplete.h"
#include "namespace.h"
#include "variable.h"

class wpl_function;
class wpl_type_struct;
class wpl_runable;
class wpl_variable;
class wpl_value;

// Class for user-defined struct types (added to namespace where declared)
class wpl_struct : public wpl_type_user_incomplete {
	private:
	bool parse_in_progress;
	bool parse_complete;
	wpl_function *destructor;

	public:
	wpl_struct (const char *name, bool no_parsing);

	virtual ~wpl_struct();
	virtual void suicide() override {
		delete this;
	}
	wpl_function *get_dtor() const {
		return destructor;
	}
	virtual wpl_value *new_instance() const;
	void parse_value(wpl_namespace *ns);
	// TODO add custom precedence choice?
	int get_precedence() const { return wpl_type_precedence_struct; }
	wpl_struct *clone() const {
		return new wpl_struct(*this);
	}
};

// Class for the type 'struct', used only once (added to global namespace)
class wpl_type_struct : public wpl_type_incomplete {
	private:

	public:
	wpl_type_struct (const char *name) : wpl_type_incomplete(name) {}
	wpl_type_struct () : wpl_type_incomplete(wpl_typename_struct) {}
	wpl_type_user_incomplete *new_instance(const char *name) const {
		return new wpl_struct(name, false);
	}
	int get_precedence() const { return wpl_type_precedence_struct; }
	virtual void suicide() {
		delete this;
	}
	wpl_identifier *clone() const {
		return new wpl_type_struct(*this);
	}
};
