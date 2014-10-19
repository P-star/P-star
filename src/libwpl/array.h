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

#include "typenames.h"
#include "type_template.h"
#include "type_precedence.h"
#include "exception.h"
#include "io.h"

#include <vector>

class wpl_value;
class wpl_state;
class wpl_namespace_session;

/**
 * @brief Array implementation
 */
class wpl_array {
	friend class wpl_program;
	friend class wpl_value_get;
	friend class wpl_value_post;

	vector<wpl_value*> array;

	public:
	void clear();
	int size() const {
		return array.size();
	}
	const wpl_value *get_readonly(int index) const;

	protected:
	wpl_array () {};
	wpl_array (const wpl_array &copy);
	void pop();
	void set(int index, wpl_value *value);
	wpl_value *get(int index);
	void push(wpl_value *value);
	void replace (wpl_array &new_array);
	void output_json(wpl_io &io);
	~wpl_array();
	void reserve(int i) {
		array.reserve(i);
	}
	void notify_destructor(wpl_state *state, wpl_namespace_session *nss, wpl_io &io);
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
	wpl_type_array(const char *name) : wpl_type_template(name) {}
	void suicide() {
		delete this;
	}

	int get_precedence() const {
		return wpl_type_precedence_array;
	}

	wpl_identifier *clone() const {
		return new wpl_type_array(*this);
	}

	wpl_type_array_instance *new_instance (const wpl_type_complete *template_type) const {
		return new wpl_type_array_instance(this, template_type);
	}
};
