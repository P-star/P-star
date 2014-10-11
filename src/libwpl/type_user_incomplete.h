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

#include "type.h"
#include "namespace.h"

/**
 * @brief Class for user-defined types, created with keywords like struct and class. These types need more parsing than just the name of the type. Incomplete types become complete when they are parsed.
 */
class wpl_type_user_incomplete : public wpl_type_complete, public wpl_namespace {
	private:

	public:
	wpl_type_user_incomplete(const char *name) :
		wpl_type_complete(name)
	{}
	virtual ~wpl_type_user_incomplete() {
#ifdef WPL_DEBUG_DESTRUCTION
		DBG("T (" << (wpl_identifier*)this << "): Destructing incomplete user type\n");
#endif
	}
	virtual void suicide() = 0;

	virtual void parse_value(wpl_namespace *parent_namespace) = 0;
};
