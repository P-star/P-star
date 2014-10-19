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

#include "block_parser.h"

#include "block.h"
#include "expression.h"
#include "parse_and_run.h"
#include "type_parse_signals.h"

#include <memory>

unique_ptr<wpl_runable> wpl_block_parser::parse_expression () {
	wpl_expression *exp = new wpl_expression();
	unique_ptr<wpl_runable> ret(exp);

	exp->load_position(get_position());
	exp->parse_value(this);
	load_position(exp->get_position());

	return ret;
}

unique_ptr<wpl_runable> wpl_block_parser::parse_block () {
	wpl_block *block = new wpl_block();
	unique_ptr<wpl_runable> ret(block);

	block->load_position(get_position());
	block->parse_value(this);
	load_position(block->get_position());

	return ret;
}

void wpl_block_parser::parse_parse_and_run(wpl_parse_and_run *block) {
	block->load_position(get_position());
	block->parse_value(this);
	load_position(block->get_position());
}

