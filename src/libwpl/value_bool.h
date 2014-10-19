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

#include "types.h"
#include "value_number.h"
#include "value_pointer.h"

#include <string>

class wpl_value_bool : public wpl_value_number<bool> {
	PRIMITIVE_CLASS_CONTENT(bool,bool,toBool(),number)

	bool toBool() override {
		return value;
	}
	int toInt() override {
		return (value ? 1 : 0);
	}
	unsigned int toUint() override {
		return (value ? 1 : 0);
	}
	float toFloat() override {
		return (value ? 1 : 0);
	}
	double toDouble() override {
		return (value ? 1 : 0);
	}
	string toString() const override {
		return (value ? string("true") : string ("false"));
	}
};
