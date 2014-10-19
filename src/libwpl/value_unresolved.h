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

#include "value_string.h"
#include "operator.h"
#include "namespace.h"

#include <string>

using namespace std;

class wpl_value_unresolved_identifier : public wpl_value_strings<string> {
	public:
	PRIMITIVE_TYPEATTR(unresolved_identifier)

	wpl_value_unresolved_identifier() : wpl_value_strings<string>() {}
	wpl_value_unresolved_identifier(const wpl_value_unresolved_identifier &copy) :
		wpl_value_strings(copy) {
	}
	wpl_value_unresolved_identifier(const char *new_value) {
		value = new_value;
	}

	wpl_value_unresolved_identifier *clone() const override {
		return new wpl_value_unresolved_identifier(*this); 
	}
	wpl_value_unresolved_identifier *clone_empty() const override {
		throw runtime_error("Empty cloning not supported for unresolved identifiers");
	}

	int do_fastop (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const wpl_operator_struct *op
			) override;

	int do_operator_recursive (
			wpl_expression_state *exp_state,
			wpl_value *final_result
			) override;

/*	wpl_value *resolve(wpl_namespace_session *nss) override;*/

	void output(wpl_io &io) override {
		cerr << "In output() of unresolved identifier '" << value << "':\n";
		throw runtime_error("Cannot output unresolved identifiers");
	}

	string toString() const {
		return value;
	}

/*	bool isUnresolved() const override {
		return true;
	}*/
};

