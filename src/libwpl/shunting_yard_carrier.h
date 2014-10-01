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

#include "runable_operator.h"

#include <memory>
#include <cstddef>

using namespace std;

struct wpl_operator_struct;
class wpl_value;

struct shunting_yard_carrier {
	wpl_value *value;
	const wpl_operator_struct *op;
	shared_ptr<wpl_runable_operator> runable;

	shunting_yard_carrier() {}
	shunting_yard_carrier(wpl_value *value) {
		this->value = value;
		this->op = NULL;
	}
	shunting_yard_carrier(const wpl_operator_struct *op) {
		this->op = op;
		this->value = NULL;
	}
	shunting_yard_carrier(wpl_value *value, const wpl_operator_struct *op) {
		this->value = value;
		this->op = op;
	}
};
