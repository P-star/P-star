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

/*#pragma once

#include "namespace_session.h"
#include "operator.h"
#include "value.h"

#include <memory>

class wpl_shuntable {
	private:
	unique_ptr<wpl_operator> my_operator;
	unique_ptr<wpl_value> my_value;

	public:
	wpl_shuntable(wpl_operator *op) : my_operator(op) {}
	wpl_shuntable(wpl_value *value) : my_value(value) {}

	wpl_operator *get_op() {
		return my_operator.get();
	}

	int run(wpl_namespace_session &nss, wpl_value **final_result) {
		return my_value->run(nss, final_result);
	}

	bool isOperator() const {
		return (my_operator.get() != nullptr);
	}

	bool isValue() const {
		return (my_value.get() != nullptr);
	}
};
*/
