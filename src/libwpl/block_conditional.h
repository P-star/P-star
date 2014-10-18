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

#include "block_intermediate.h"
#include <memory>

class wpl_namespace;
class wpl_parse_and_run;
class wpl_block_conditional_state;

class wpl_block_conditional : public wpl_block_intermediate {
	protected:
	unique_ptr<wpl_runable> run_condition;
	bool check_run(wpl_block_conditional_state *block_state);

	void find_and_parse_complete_type();

	public:
	wpl_block_conditional(wpl_parse_and_run *block) :
		wpl_block_intermediate(block)
	{}
	wpl_block_conditional(const wpl_block_conditional &copy) :
		wpl_block_intermediate(copy),
		run_condition()
	{}
	virtual ~wpl_block_conditional() {}

	virtual void parse_value(wpl_namespace *ns) = 0;
};
