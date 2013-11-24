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

#include "array.h"
#include "value.h"
#include "value_array.h"
#include <iostream>
	
wpl_array::wpl_array (const wpl_array &copy) {
	array.reserve(copy.array.size());
	for (wpl_value *value : copy.array) {
		array.push_back(value->clone());
	}
}

void wpl_array::set(int index, wpl_value *value) {
	if (index >= array.size()) {
		array.resize(index+1, NULL);
	}
	if (array[index] != NULL) {
		delete array[index];
	}
	array[index] = value;
}

void wpl_array::push(wpl_value *value) {
	set(array.size(), value);
}

wpl_array::~wpl_array() {
	for (wpl_value *value : array) {
		delete value;
	}
	array.clear();
}

wpl_value *wpl_array::get(int index) {
	if (index < 0) {
		throw runtime_error("Attempted to read from array with negative index");
	}
	else if (index >= array.size()) {
		set(index, NULL);
	}
	return array[index];
}

void wpl_array::replace (wpl_array &new_array) {
	for (wpl_value *value : array) {
		delete value;
	}
	array.clear();
	array.reserve(new_array.size());
	for (wpl_value *value : new_array.array) {
		array.push_back(value->clone());
	}
}

void wpl_array::output_json () {
	cout << "[";
	bool first = true;
	for (wpl_value *value : array) {
		if (!first) {
			cout << ", ";
		}
		value->output_json();
		first = false;
	}
	cout << "]";
}

wpl_value *wpl_type_array_instance::new_instance() const {
	return new wpl_value_array(template_type);
}
