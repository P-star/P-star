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
#include "value_hash.h"
#include "exception.h"
#include "value_get.h"
#include "array.h"

#include <sstream>
#include <tuple>
#include <memory>
#include <unordered_map>

#include <mimetic/mimetic.h>

class wpl_operator_struct;

class wpl_value_post : public wpl_value {
	private:

	void parse(wpl_io &io);

	shared_ptr<bool> did_parse;
	shared_ptr<wpl_value_hash> hash;
	shared_ptr<wpl_value_get> value_get;
	shared_ptr<bool> use_get;

	// Initialized in .cpp
	static wpl_type_array_instance type_complete_array;
	static wpl_type_hash_instance type_complete_hash;

	void parse_entity (mimetic::MimeEntity *me);

	public:
	PRIMITIVE_TYPEINFO(get)
	wpl_value_post(int dummy) :
		hash(new wpl_value_hash (&type_complete_hash, &type_complete_array)),
		value_get(new wpl_value_get (0)),
		did_parse(new bool(false)),
		use_get(new bool (false))
	{}
	wpl_value_post *clone() const { return new wpl_value_post(0); };
	wpl_value_post *clone_empty() const { return new wpl_value_post(0); };

	int do_operator (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
	);

	void set_weak (wpl_value *value) override {
		wpl_value_post *post = dynamic_cast<wpl_value_post*>(value);
		if (!post) {
			ostringstream msg;
			msg << "Cannot set POST value to value of type " << value->get_type_name() << endl;
			throw runtime_error(msg.str());
		}

		*this = *post;
	}
};
