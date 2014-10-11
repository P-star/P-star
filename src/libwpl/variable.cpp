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

#include "value.h"
#include "variable.h"

#include <utility>

wpl_variable::wpl_variable (wpl_value *new_value, const char *name, int access_flags) :
	value(new_value),
	wpl_identifier_access_holder(name, access_flags)
{
	is_static = false;
	is_protected = false;
	value->set_flags(wpl_value_is_constant);
}

const char *wpl_variable::get_type_name() const {
	return value->get_type_name();
}

wpl_value *wpl_variable::get_value() {
	if (is_protected){
		cerr << "While trying to get_value() in variable " << get_name() << endl;
		throw runtime_error("No access to protected variable");
	}
	return value;
}

void wpl_variable::set_weak (wpl_value *value) {
	if (is_protected){
		cerr << "While trying to set_weak() variable " << get_name() << endl;
		throw runtime_error("No access to protected variable");
	}
	this->value->set_weak(value);
}

bool wpl_variable::set_strong (wpl_value *value) {
	if (is_protected){
		cerr << "While trying to set_strong() variable " << get_name() << endl;
		throw runtime_error("No access to protected variable");
	}
	return this->value->set_strong(value);
}

wpl_value *wpl_variable::clone_value() const {
	if (is_protected){
		cerr << "While trying to clone_value() in variable " << get_name() << endl;
		throw runtime_error("No access to protected variable");
	}
	return value->clone();
}

wpl_variable_holder::~wpl_variable_holder() {
	value->suicide();
}

void wpl_variable_holder::set_value(wpl_value *new_value) {
	value->suicide();
	value = new_value;
}

wpl_variable_holder::wpl_variable_holder (const char *name, wpl_value *new_value, int access_flags) :
	wpl_variable (new_value, name, access_flags)
{}

wpl_variable_holder::wpl_variable_holder (const wpl_variable_holder &copy) :
	wpl_variable(copy)
{
	if (copy.is_protected){
		cerr << "In copy constructor of variable with const argument of variable " << copy.get_name() << endl;
		throw runtime_error("No access to protected variable");
	}
	value = value->clone();//copy.clone_value();
	value->set_flags(wpl_value_is_constant);
	is_protected = false;
	is_static = copy.is_static;
}

wpl_variable_holder *wpl_variable_holder::clone() const {
	if (is_protected){
		cerr << "While trying to clone() in variable " << get_name() << endl;
		throw runtime_error("No access to protected variable");
	}
	return new wpl_variable_holder(*this);
}
