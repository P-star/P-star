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

#include "variable.h"
#include "type_parse_signals.h"
#include "parseable.h"

class wpl_namespace;

class wpl_modifier : public wpl_parseable_identifier {
	protected:
	wpl_modifier (const char *name) :
		wpl_parseable_identifier(name)
	{}
	void suicide() {
		delete this;
	}
	void parse_value (wpl_namespace *parent_namespace);
	virtual void modify_access_flags(wpl_type_begin_declaration &e) = 0;
};

class wpl_modifier_protected : public wpl_modifier {
	public:
	wpl_modifier_protected() : wpl_modifier ("protected")
	{}
	void modify_access_flags(wpl_type_begin_declaration &e) override {
		e.set_flags(WPL_VARIABLE_ACCESS_PROTECTED);
	}
};

class wpl_modifier_private : public wpl_modifier {
	public:
	wpl_modifier_private() : wpl_modifier ("private")
	{}
	void modify_access_flags(wpl_type_begin_declaration &e) override {
		e.set_flags(WPL_VARIABLE_ACCESS_PRIVATE);
	}
};

class wpl_modifier_public : public wpl_modifier {
	public:
	wpl_modifier_public() : wpl_modifier ("public")
	{}
	void modify_access_flags(wpl_type_begin_declaration &e) override {
		e.set_flags(WPL_VARIABLE_ACCESS_PUBLIC);
	}
};
