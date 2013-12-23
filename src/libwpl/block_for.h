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

#include "block.h"
#include "operator.h"
#include "expression.h"

class wpl_namespace;
class wpl_value;

class wpl_block_for : public wpl_block {
	private:
	unique_ptr<wpl_expression> exp_init;
	unique_ptr<wpl_expression> exp_condition;
	unique_ptr<wpl_expression_par_enclosed> exp_increment;

	public:
	wpl_block_for();
	int run(wpl_state *state, wpl_value *final_result) override;
	void parse_value(wpl_namespace *ns);
};
