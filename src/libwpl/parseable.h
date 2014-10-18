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

#include "identifier.h"
#include "suicidal.h"
#include "matcher.h"

class wpl_namespace;

class wpl_parseable : public wpl_suicidal, public wpl_matcher {
	public:
	virtual ~wpl_parseable() {}

	virtual void parse_value (wpl_namespace *parent_namespace) = 0;
};

/**
 * @brief Used by the type system
 */
class wpl_parseable_identifier : public wpl_parseable, public wpl_identifier {
	public:
	wpl_parseable_identifier(const char *name) :
		wpl_parseable(),
		wpl_identifier(name)
	{}
	virtual ~wpl_parseable_identifier() {}
};
