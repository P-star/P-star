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

#include "block_foreach.h"
#include "block_while.h"
#include "block_if.h"
#include "block_for.h"

class wpl_text_chunk;

template<typename T> class wpl_text_chunk_type : public wpl_parseable {
	public:
	wpl_text_chunk_type(const char *name) : 
		wpl_parseable(name)
	{}
	virtual ~wpl_text_chunk_type() {}

	wpl_text_chunk *new_instance() const {
		return new T;
	}
};

namespace wpl_text_chunk_types {
	class block_foreach : public wpl_text_chunk_type<wpl_block_foreach> {
		public:
		block_foreach() : wpl_text_chunk_type("foreach") {}
	};
	class block_while : public wpl_text_chunk_type<wpl_block_while> {
		public:
		block_while() : wpl_text_chunk_type("while") {}
	};
	class block_if : public wpl_text_chunk_type<wpl_block_if> {
		public:
		block_if() : wpl_text_chunk_type("if") {}
	};
	class block_for : public wpl_text_chunk_type<wpl_block_for> {
		public:
		block_for() : wpl_text_chunk_type("for") {}
	};

	void add_types_to_namespace(wpl_namespace *ns);
};

