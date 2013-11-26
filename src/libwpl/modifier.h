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

#include "parseable.h"

class wpl_namespace;

class wpl_modifier : public wpl_parseable {
	public:
	wpl_modifier (const char *name) : wpl_parseable(name) {
	}
};

class wpl_modifier_protected : public wpl_modifier {
	public:
	wpl_modifier_public () : wpl_modifier ("protected") {
	}
};

class wpl_modifier_private : public wpl_modifier {
	public:
	wpl_modifier_public () : wpl_modifier ("private") {
	}
};
