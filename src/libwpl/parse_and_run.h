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

#include "parseable.h"
#include "runable.h"

#include <cstring>

class wpl_parse_and_run : public wpl_parseable, public wpl_runable {
	private:
	char name[WPL_VARNAME_SIZE+1];

	public:
	wpl_parse_and_run(const char *name) :
		wpl_parseable(),
	       	wpl_runable()
	{
		strncpy(this->name, name, WPL_VARNAME_SIZE);
	}
	wpl_parse_and_run(const wpl_parse_and_run &copy) :
		wpl_parseable(copy),
		wpl_runable(copy)
	{
		strncpy(this->name, copy.name, WPL_VARNAME_SIZE);
	}
	virtual ~wpl_parse_and_run() {}
	virtual const char *get_name() const {
		return name;
	}
	virtual void set_expect_blockstart() {}
	virtual wpl_parse_and_run *new_instance() const = 0;
};
