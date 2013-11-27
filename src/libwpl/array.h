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

#include "types.h"
#include "exception.h"

#include <vector>

class wpl_value;

/**
 * @brief Array implementation
 */
class wpl_array {
	vector<wpl_value*> array;

	public:
	void set(int index, wpl_value *value);
	void push(wpl_value *value);

	protected:
	wpl_array () {};
	wpl_array (const wpl_array &copy);
	wpl_value *get(int index);
	void replace (wpl_array &new_array);
	void output_json();
	~wpl_array();
	int size() {
		return array.size();
	}
	void reserve(int i) {
		array.reserve(i);
	}
};

/**
 * @brief Class for a complete array type, with template type specified
 */
class wpl_type_array_instance : public wpl_type_complete_template {
	public:
	wpl_type_array_instance(
			const wpl_type_template *mother_type,
			const wpl_type_complete *template_type
			) :
		wpl_type_complete_template (mother_type, template_type)
	{}

	void suicide() {
		delete this;
	}

	wpl_identifier *clone() const {
		return new wpl_type_array_instance(*this);
	}

	wpl_value *new_instance() const;
};

/**
 * @brief Class for the array template type
 */
class wpl_type_array : public wpl_type_template {
	public:
	wpl_type_array () : wpl_type_template(wpl_typename_array) {}

	void suicide() {
		delete this;
	}

	int get_precedence() const {
		return wpl_type_precedence_array;
	}

	wpl_identifier *clone() const {
		return new wpl_type_array(*this);
	}

	wpl_type_complete_template *new_instance (const wpl_type_complete *template_type) const {
		return new wpl_type_array_instance(this, template_type);
	}
};
