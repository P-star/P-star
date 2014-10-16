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

#include "namespace.h"
#include "matcher.h"
#include "runable.h"

#include <vector>

class wpl_block_parser : public wpl_namespace, public wpl_matcher {
	private:
	int parse_statement_to_semicolon (const char **target, uint32_t match, uint32_t ignore);

	protected:
	unique_ptr<wpl_runable> parse_expression (wpl_namespace *ns);
	unique_ptr<wpl_runable> parse_block (wpl_namespace *ns);
	unique_ptr<wpl_runable> parse_pragma (wpl_namespace *ns);

	unique_ptr<wpl_runable> parse_foreach(wpl_namespace *ns);
	unique_ptr<wpl_runable> parse_for(wpl_namespace *ns);
	unique_ptr<wpl_runable> parse_while(wpl_namespace *ns);
	unique_ptr<wpl_runable> parse_if(wpl_namespace *ns);

	unique_ptr<wpl_runable> parse_text(wpl_namespace *ns);

	void parse_parseable(wpl_namespace *ns, wpl_parseable *parseable);
	void parse_comment();
	
};
