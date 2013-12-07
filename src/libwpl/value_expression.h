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

#include "value_holder.h"
#include "operator.h"
#include "namespace.h"

#include <memory>

using namespace std;

class wpl_expression;

class wpl_value_expression : public wpl_value {
	private:
	wpl_expression *my_expression;

	public:
	PRIMITIVE_TYPEINFO(expression)

	wpl_value_expression(const wpl_value_expression &copy) {
		throw runtime_error("Cannot clone expression value");
	}
	wpl_value_expression(wpl_expression *exp) {
		my_expression = exp;
	}
	~wpl_value_expression();

	wpl_value_expression *clone() const override {
		throw runtime_error("Cloning not supported for expression values");
	}
	wpl_value_expression *clone_empty() const override {
		throw runtime_error("Empty cloning not supported for expression values");
	}

	int run (wpl_namespace_session &nss, wpl_value **res);

	void dump() override;
};

*/
