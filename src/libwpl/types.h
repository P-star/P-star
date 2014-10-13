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
#include "typenames.h"
#include "type_precedence.h"

#define DECLARE_COMPLETE_TYPE(name)							\
	class wpl_type_##name : public wpl_type_complete {				\
		public:									\
		wpl_type_##name() : wpl_type_complete(wpl_typename_##name) {}		\
		wpl_type_##name(const char *type) : wpl_type_complete(type) {}		\
		int get_precedence() const { return wpl_type_precedence_##name; }	\
		wpl_value *new_instance () const;					\
		wpl_identifier *clone() const { return new wpl_type_##name(*this); }	\
		void suicide() { delete this; }						\
	};										\
	extern const wpl_type_##name *wpl_type_global_##name;

DECLARE_COMPLETE_TYPE(void);
DECLARE_COMPLETE_TYPE(bool);
DECLARE_COMPLETE_TYPE(int);
DECLARE_COMPLETE_TYPE(llint);
DECLARE_COMPLETE_TYPE(uint);
DECLARE_COMPLETE_TYPE(lluint);
DECLARE_COMPLETE_TYPE(float);
DECLARE_COMPLETE_TYPE(double);
DECLARE_COMPLETE_TYPE(string);

DECLARE_COMPLETE_TYPE(env);
DECLARE_COMPLETE_TYPE(get);
DECLARE_COMPLETE_TYPE(post);
DECLARE_COMPLETE_TYPE(time);

#ifndef WIN32
DECLARE_COMPLETE_TYPE(stdin);
#endif

DECLARE_COMPLETE_TYPE(line);
DECLARE_COMPLETE_TYPE(auto);

void wpl_types_add_all_to_namespace(wpl_namespace *parent_namespace);
