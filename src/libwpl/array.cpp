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

#include "array.h"
#include "value.h"
#include "value_array.h"

wpl_array::wpl_array (const wpl_array &copy) {
	array.reserve(copy.array.size());
	for (wpl_value *value : copy.array) {
		array.push_back(value->clone());
	}
}

void wpl_array::pop() {
	wpl_value *value = array.back();
	array.pop_back();
	if (value) {
		delete value;
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

void wpl_array::clear() {
	for (wpl_value *value : array) {
		if (value) {
			delete value;
		}
	}
	array.clear();
}

wpl_array::~wpl_array() {
	clear();
}

const wpl_value *wpl_array::get_readonly(int index) const {
	if (index < 0) {
		throw runtime_error("Attempted to read from array with negative index");
	}
	else if (index >= array.size()) {
		throw runtime_error("Attempted to read from outside the array in read only mode");
	}
	return array[index];
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
		if (value) {
			delete value;
		}
	}
	array.clear();
	array.reserve(new_array.size());
	for (wpl_value *value : new_array.array) {
		if (value) {
			array.push_back(value->clone());
		}
		else {
			array.push_back(NULL);
		}
	}
}

void wpl_array::output_json (wpl_io &io) {
	io << "[";
	bool first = true;
	for (wpl_value *value : array) {
		if (value) {

			if (!first) {
				io << ",";
			}

			if (value->isStruct() || value->isArray() || value->isPointer()) {
				value->output_json(io);
			}
			else {
				io << "\"";
				value->output_json(io);
				io << "\"";
			}

			first = false;
		}
	}
	io << "]";
}

wpl_value *wpl_type_array_instance::new_instance() const {
	return new wpl_value_array(this, template_type);
}
void wpl_array::notify_destructor(wpl_state *state, wpl_namespace_session *nss, wpl_io &io) {
	for (auto *value : array) {
		if (value) {
			value->notify_destructor(state, nss, io);
		}
	}
}
