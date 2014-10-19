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
#include "parse_and_run.h"
#include "runable.h"

#include <vector>
#include <memory>

class wpl_parseable_identifier;

class wpl_block_parser : public wpl_parse_and_run, public wpl_namespace {
	private:
	vector<unique_ptr<wpl_parse_and_run>> parse_and_run_blocks;

	int parse_statement_to_semicolon (const char **target, uint32_t match, uint32_t ignore);

	protected:
	wpl_block_parser() : wpl_parse_and_run("BLOCK_PARSER"), wpl_namespace() {}
	wpl_block_parser (const wpl_block_parser &copy) :
		wpl_parse_and_run(copy),
		wpl_namespace() // DO NOT clone namespace
	{}

	/*
	   TODO
	   put these in namespace with new parseable types, like the
	   text chunk system
	   */
	unique_ptr<wpl_runable> parse_expression ();
	unique_ptr<wpl_runable> parse_block ();

//	const wpl_parse_and_run *find_parse_and_run(const char *name);
	void parse_parse_and_run(wpl_parse_and_run *block);
	
};
