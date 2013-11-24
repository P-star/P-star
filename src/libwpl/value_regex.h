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

#include "typenames.h"
#include "value_holder.h"
#include "exception.h"

#include <boost/regex.hpp>

struct wpl_operator_struct;

class wpl_value_regex : public wpl_value {
	private:
	boost::regex my_regex;

	public:
	PRIMITIVE_TYPEINFO(regex)
	wpl_value_regex(const char *regex_string) :
		my_regex(regex_string)
	{}

	wpl_value *clone() const {
		return new wpl_value_regex(*this);
	}

	bool do_pattern_match (wpl_expression_state *exp_state, string &subject) override;

	int do_operator (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
	);
};
