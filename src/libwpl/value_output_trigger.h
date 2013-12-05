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

#include "io.h"
#include "value.h"

#include <sstream>

using namespace std;

class wpl_value_output_trigger : public wpl_value {
	private:
	wpl_io *io;

	public:
	int get_precedence() const { return wpl_type_precedence_output_trigger; };
	const char *get_type_name() const { return wpl_typename_output_trigger; }

	wpl_value_output_trigger (wpl_io &io) {
		this->io = &io;
	}

	wpl_value *clone() const {
		return new wpl_value_output_trigger(*this);
	};

	int finalize_expression (wpl_expression_state *exp_state, wpl_value *value) {
		value->output(*io);
		return WPL_OP_OK;
	}
};

