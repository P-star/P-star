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

#include "block_parser.h"

#include "expression.h"
#include "variable.h"
#include "user_function.h"
#include "block_if.h"
#include "block_while.h"
#include "block_for.h"
#include "block_foreach.h"
#include "block_conditional.h"
#include "block_intermediate.h"
#include "pragma.h"
#include "text.h"
#include "parseable.h"

#include "type_parse_signals.h"

#include <memory>

unique_ptr<wpl_runable> wpl_block_parser::parse_expression (wpl_namespace *ns) {
	wpl_expression *exp = new wpl_expression();
	unique_ptr<wpl_runable> ret(exp);

	exp->load_position(get_position());
	exp->parse_value(ns);
	load_position(exp->get_position());

	return ret;
}

unique_ptr<wpl_runable> wpl_block_parser::parse_block (wpl_namespace *ns) {
	wpl_block *block = new wpl_block();
	unique_ptr<wpl_runable> ret(block);

	block->set_parent_namespace(ns);
	block->load_position(get_position());
	block->parse_value(block);
	load_position(block->get_position());

	return ret;
}

unique_ptr<wpl_runable> wpl_block_parser::parse_pragma (wpl_namespace *ns) {
	char name[WPL_VARNAME_SIZE];

	int len = search (CAPITAL_WORD, 0, false);
	check_varname_length(len);
	get_string(name, len);

	const wpl_pragma *_pragma = find_pragma(name);

	if (!_pragma) {
		cerr << "While parsing pragma name '" << name << "':\n";
		revert_string(len);
		THROW_ELEMENT_EXCEPTION("Unkown pragma type");
	}

	wpl_pragma *pragma = _pragma->clone();
	unique_ptr<wpl_runable> ret(pragma);

	pragma->load_position(get_position());
	pragma->parse_value(ns);
	load_position(pragma->get_position());

	return ret;
}

#define COND_BLOCK_DEF(name,arg) \
	unique_ptr<wpl_runable> wpl_block_parser::parse_##name(wpl_namespace *ns) {	\
		wpl_block_conditional *block = new wpl_block_##name(arg);		\
		unique_ptr<wpl_runable> ret(block);					\
		block->load_position(get_position());					\
		block->parse_value(ns);							\
		load_position(block->get_position());					\
		return ret;								\
	}

COND_BLOCK_DEF(while,)
COND_BLOCK_DEF(if,wpl_block_if::F_IF)
COND_BLOCK_DEF(foreach,)
COND_BLOCK_DEF(for,)

/**
 * @brief Parse a TEXT{}-block
 */
unique_ptr<wpl_runable> wpl_block_parser::parse_text(wpl_namespace *ns) {
	ignore_blockstart();
	wpl_text *text = new wpl_text();
	unique_ptr<wpl_runable> ret(text);
	text->load_position(get_position());
	text->parse_value(ns);
	load_position(text->get_position());

	return ret;
}

/**
 * @brief Parse a parseable with unknown, but funky syntax. The parseable can add something to our namespace if it wants to.
 */
void wpl_block_parser::parse_parseable(wpl_namespace *ns, wpl_parseable *parseable) {
	parseable->load_position(get_position());

	try {
		try {
			parseable->parse_value(ns);
		}
		catch (wpl_type_begin_variable_declaration &e) {
			e.create_variable(ns);
			load_position(e.get_position_at_name());
		}
	}
	catch (wpl_type_begin_function_declaration &e) {
		e.load_position(parseable->get_position());
		e.parse_value(ns);
		load_position(e.get_position());
	}
}

/**
 * @brief Parse a comment block ending with * / (without the space, can't write it here in C++ :-) )
 */
void wpl_block_parser::parse_comment() {
	wpl_matcher_position start = get_position();

	while (get_letter ('*', NON_ASTERISK)) {
		if (ignore_letter ('/')) {
			return;
		}
	}

	load_position(start);
	revert_string(2);

	THROW_ELEMENT_EXCEPTION("Could not find comment end for this comment");
}

