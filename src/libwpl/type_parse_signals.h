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

#include "matcher.h"

class wpl_namespace;
class wpl_type_complete;

class wpl_type_begin_declaration {
	protected:
	wpl_type_complete *type;
	string name;
	int access_flags;

	wpl_type_begin_declaration (
		wpl_type_complete *_type,
		const char *_name,
		int _access_flags
	) :
		type(_type),
		name(_name),
		access_flags(_access_flags)
	{}
	public:
	void set_flags(int access_flags) {
		this->access_flags = access_flags;
	}
};

/**
 * @brief When a complete type finds word and a '(' after itself, this is thrown
 */
class wpl_type_begin_function_declaration : public wpl_type_begin_declaration, public wpl_matcher {
	public:
	wpl_type_begin_function_declaration(
		wpl_type_complete *type,
		const char *name,
		const wpl_matcher_position &pos
	);
	void parse_value (wpl_namespace *parent_namespace);
};

/**
 * @brief When a complete type finds a word after itself, this is thrown
 */
class wpl_type_begin_variable_declaration : public wpl_type_begin_declaration, public wpl_matcher {
	private:
	const wpl_matcher_position position_at_name;

	public:
	wpl_type_begin_variable_declaration (
			wpl_type_complete *type,
			const char *name,
			const wpl_matcher_position &position_at_name,
			const wpl_matcher_position &position_after_name
	);
	void create_variable(wpl_namespace *parent_namespace);
	const wpl_matcher_position get_position_at_name() {
		return position_at_name;
	}
};

/**
 * @brief This is thrown when structs are completely parsed
 */
class wpl_type_end_statement {
	private:
	wpl_matcher_position end_pos;
	public:
	wpl_type_end_statement(const wpl_matcher_position &pos) :
		end_pos(pos)
	{}
	const wpl_matcher_position get_position() {
		return end_pos;
	}
};


