/*

-------------------------------------------------------------

Copyright (c) MMXIV Atle Solbakken
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

#include "type_template.h"
#include "exception.h"
#include "io.h"
#include "value.h"
#include <stdexcept>

/**
 * @brief Pointer implementation
 */
class wpl_pointer {
	private:
	wpl_value *value;

	void deregister() {
		if (value) {
			value->remove_pointer(this);
			value = NULL;
		}
	}

	protected:
	inline wpl_value *get_value() {
		if (value == NULL) {
			throw runtime_error("Attempted to use pointer, but it is not set to anything yet");
		}
		return value;
	}

	void set_value(wpl_value *value) {
		deregister();
		this->value = value;
		value->register_pointer(this);
	}

	wpl_pointer() :
		value(NULL)
	{}

	wpl_pointer(wpl_value *value) :
		value(value)
	{
		if (value) {
			value->register_pointer(this);
		}
	}

	wpl_pointer(const wpl_pointer &copy) {
		value = copy.value;

		if (value) {
			value->register_pointer(this);
		}
	}

	~wpl_pointer() {
		deregister();
	}

	public:
	void value_dies_now() {
		value = NULL;
	}
};

/**
 * @brief Class for a complete pointer type, with template type specified
 */
class wpl_type_pointer_instance : public wpl_type_complete_template {
	public:
	wpl_type_pointer_instance(
			const wpl_type_template *mother_type,
			const wpl_type_complete *template_type
			) :
		wpl_type_complete_template (mother_type, template_type)
	{}

	void suicide() {
		delete this;
	}

	wpl_identifier *clone() const {
		return new wpl_type_pointer_instance(*this);
	}

	wpl_value *new_instance() const;
};

/**
 * @brief Class for the pointer template type
 */
class wpl_type_pointer : public wpl_type_template {
	public:
	wpl_type_pointer () : wpl_type_template(wpl_typename_pointer) {}
	wpl_type_pointer(const char *name) : wpl_type_template(name) {}
	void suicide() {
		delete this;
	}

	int get_precedence() const {
		return wpl_type_precedence_pointer;
	}

	wpl_identifier *clone() const {
		return new wpl_type_pointer(*this);
	}

	wpl_type_complete_template *new_instance (const wpl_type_complete *template_type) const {
		return new wpl_type_pointer_instance(this, template_type);
	}
};
