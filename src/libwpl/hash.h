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

#include "type_template.h"
#include "exception.h"
#include "value.h"
#include "io.h"

#include <memory>
#include <map>

class wpl_value;
class wpl_value_array;

/**
 * @brief Hash implementation
 */
class wpl_hash {
	map<string,unique_ptr<wpl_value>> hash;

	public:
	void erase(string &key);
	void set(string &key, wpl_value *value);
	wpl_value *get(string &key);
	void get_keys(wpl_value_array *array);

	protected:
	wpl_hash () {};
	wpl_hash (const wpl_hash &copy);
	~wpl_hash();
	void replace (wpl_hash &new_hash);
	void output_json(wpl_io &io);
	int size() {
		return hash.size();
	}
	void notify_destructor (wpl_state *state, wpl_namespace_session *nss, wpl_io &io);
};

/**
 * @brief Class for a complete hash type, with template type specified
 */
class wpl_type_hash_instance : public wpl_type_complete_template {
	public:
	wpl_type_hash_instance(
			const wpl_type_template *mother_type,
			const wpl_type_complete *template_type
			) :
		wpl_type_complete_template (mother_type, template_type)
	{}

	void suicide() {
		delete this;
	}

	wpl_identifier *clone() const {
		return new wpl_type_hash_instance(*this);
	}

	wpl_value *new_instance() const;
};

/**
 * @brief Class for the hash template type
 */
class wpl_type_hash : public wpl_type_template {
	public:
	wpl_type_hash () : wpl_type_template(wpl_typename_hash) {}

	void suicide() {
		delete this;
	}

	int get_precedence() const {
		return wpl_type_precedence_hash;
	}

	wpl_identifier *clone() const {
		return new wpl_type_hash(*this);
	}

	wpl_type_complete_template *new_instance (const wpl_type_complete *template_type) const {
		return new wpl_type_hash_instance(this, template_type);
	}
};

