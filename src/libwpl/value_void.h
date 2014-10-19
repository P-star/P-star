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

#include "value_holder.h"
#include "operator.h"
#include "value.h"
#include "void.h"

class wpl_value_void : public wpl_value_holder<wpl_void> {
	public:
	PRIMITIVE_TYPEINFO(void)
	wpl_value_void *clone() const {
		return new wpl_value_void(*this);
	}

	/* Allow output, but do nothing */
	void output(wpl_io &io) override {}

	/* Allow finalize, but don't set */
	int finalize_expression(wpl_expression_state *exp_state, wpl_value *final_result) override {
		return WPL_OP_NO_RETURN;
	}

	/* Return 0 for toInt() */
	int toInt() override {
		return 0;
	}
	string toString() const override {
		return string();
	}
};

