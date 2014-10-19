/*

-------------------------------------------------------------

Copyright (c) MMXIV Atle Solbakken
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
#include "scene.h"
#include "matcher.h"
#include "namespace.h"

#include <iostream>
#include <list>
#include <memory>

using namespace std;

class wpl_namespace;

class wpl_parser : public wpl_matcher {
	private:

	/*
	   When we run in Apache, this pointer goes out
	   of scope after the first time the program is run,
	   and we should not use it at run-time.
	   */
	wpl_io *io;

	list<unique_ptr<wpl_parser>> includes;
	int num_parents;

	char varname_tmp[WPL_VARNAME_SIZE+1];

	void syntax_error();

	void parse_include(wpl_namespace *parent_namespace);
	void parse_scene(wpl_namespace *parent_namespace);
	void parse_template(wpl_namespace *parent_namespace);
	void first_level(wpl_namespace *parent_namespace);

	string file_content;

	public:

	wpl_parser (wpl_io &io, int num_parents);
	~wpl_parser ();
	void parse_file(wpl_namespace *parent_namespace, const char *filename);
	static wpl_matcher_position parse_parseable_identifier(
		wpl_namespace *ns,
		wpl_matcher *matcher,
		wpl_parseable_identifier *parseable
	);
	static void parse_comment(wpl_matcher *matcher);
};

