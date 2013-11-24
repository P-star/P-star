/*

-------------------------------------------------------------

Copyright (c) MMIII Atle Solbakken
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
/*
#pragma once

#include "matcher.h"

#include <iostream>
#include <string>

class wpl_value;

class wpl_json : public matcher {
	private:
	int elements_count;

	public:
	wpl_json() {
		elements_count = 0;
		cout << "{\n";
	}
	~wpl_json() {
		cout << "}\n";
	}
	static void escape_string (string &result, string &input);
	void output (wpl_value *id, wpl_value *var);
};

*/