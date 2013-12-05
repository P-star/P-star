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

#include "hash.h"
#include "value.h"
#include "value_hash.h"
#include "output_json.h"

#include <memory>
#include <iostream>
	
wpl_hash::wpl_hash (const wpl_hash &copy) {
	for (auto &my_pair : copy.hash) {
		hash[my_pair.first] = unique_ptr<wpl_value>(my_pair.second->clone());
	}
}

void wpl_hash::set(string &key, wpl_value *value) {
	hash[key] = unique_ptr<wpl_value>(value);
}

wpl_hash::~wpl_hash() {
}

wpl_value *wpl_hash::get(string &key) {
	return hash[key].get();
}

void wpl_hash::replace (wpl_hash &new_hash) {
	hash.clear();
	for (auto &my_pair : new_hash.hash) {
		hash[my_pair.first] = unique_ptr<wpl_value>(my_pair.second->clone());
	}
}

void wpl_hash::output_json (wpl_io &io) {
	io << "{\n";
	bool first = true;
	wpl_output_json output_json;
	for (auto &my_pair : hash) {
		if (!first) {
			io << ", ";
		}
		output_json.output_json(io, my_pair.first.c_str(), my_pair.first.size());
		io << ": ";
		my_pair.second->output_json(io);
		first = false;
	}
	io << "}\n";
}

wpl_value *wpl_type_hash_instance::new_instance() const {
	return new wpl_value_hash(template_type);
}
