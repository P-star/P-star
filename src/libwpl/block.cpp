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

#include "exception.h"
#include "blocks.h"
#include "debug.h"
#include "block.h"

#include "block_state.h"
#include "expression.h"
#include "variable.h"
#include "user_function.h"
#include "block_if.h"
#include "block_while.h"
#include "pragma.h"
#include "text.h"
#include "value_bool.h"
#include "parseable.h"

#include "modifier.h"

#include <utility>
#include <cstring>
#include <cstdio>

using namespace std;

wpl_block::wpl_block() {
	run_condition = NULL;
	wpl_modifier_add_all_to_namespace(this);
}

wpl_block::~wpl_block() {
#ifdef WPL_DEBUG_DESTRUCTION
	DBG("B (" << this << "): Destructing wpl_block\n");
#endif
	if (run_condition) {
		delete run_condition;
	}
}

wpl_state *wpl_block::new_state(wpl_namespace_session *nss, wpl_io *io) {
	return new wpl_block_state (nss, io, this);
}

void wpl_block::append_child (wpl_runable *element) {
	child_elements.push_back(unique_ptr<wpl_runable>(element));
}

void wpl_block::append_child_position() {
	child_positions.emplace_back(get_position());
}

void wpl_block::parse_expression (wpl_namespace *ns) {
	wpl_expression *exp = new wpl_expression();

	append_child(exp);

	exp->load_position(get_position());
	exp->parse_value(ns);
	load_position(exp->get_position());
}

void wpl_block::parse_block (wpl_namespace *ns) {
	wpl_block *block = new wpl_block();
	append_child (block);

	block->set_parent_namespace(ns);
	block->load_position(get_position());
	block->parse_value(block);
	load_position(block->get_position());
}

void wpl_block::parse_pragma (wpl_namespace *ns) {
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
	append_child(pragma);

	pragma->load_position(get_position());
	pragma->parse_value(ns);
	load_position(pragma->get_position());
}

void wpl_block::parse_while(wpl_namespace *ns) {
	wpl_block_while *block = new wpl_block_while();
	append_child(block);

	wpl_expression *exp = new wpl_expression_par_enclosed();
	block->set_run_condition(exp);

	exp->load_position(get_position());
	exp->parse_value(ns);
	load_position(exp->get_position());

	ignore_blockstart();

	block->set_parent_namespace(ns);
	block->load_position(get_position());
	block->parse_value(block);
	load_position(block->get_position());
}

void wpl_block::parse_if_else_sequenze(wpl_namespace *ns) {
#ifdef WPL_DEBUG_BLOCKS
	DBG("B (" << this << "): Parse if-else-seq\n");
#endif
	wpl_block_if *block;
	wpl_expression_par_enclosed *exp; 

	block = new wpl_block_if();
	append_child (block);

	exp = new wpl_expression_par_enclosed();
	block->set_run_condition(exp);

	exp->load_position(get_position());
	exp->parse_value(ns);
	load_position(exp->get_position());

	ignore_blockstart();

	block->set_parent_namespace(ns);
	block->load_position(get_position());
	block->parse_value(block);
	load_position(block->get_position());

	wpl_block_if *block_if_prev = block;

	ignore_string_match(WHITESPACE, 0);

	while (ignore_string (wpl_blockname_else_if)) {
		// Found an else if
		wpl_block_if *block_else_if = new wpl_block_if();
		block_if_prev->set_next_else_if(block_else_if);
		exp = new wpl_expression_par_enclosed();
		block_else_if->set_run_condition(exp);

		ignore_blockstart();
		block_else_if->set_parent_namespace(ns);
		block_else_if->load_position(get_position());
		block_else_if->parse_value(block_else_if);
		load_position(block_else_if->get_position());

		block_if_prev = block_else_if;

		ignore_string_match(WHITESPACE, 0);
	}

	if (ignore_string (wpl_blockname_else)) {
		// Found an else, last loop
		wpl_block_if *block_else;

		block_else = new wpl_block_if();
		block_if_prev->set_next_else_if(block_else);

		ignore_blockstart();

		block_else->set_parent_namespace(ns);
		block_else->load_position(get_position());
		block_else->parse_value(block_else);
		load_position(block_else->get_position());
	}
}

/**
 * @brief Parse a TEXT{}-block
 */
void wpl_block::parse_text(wpl_namespace *ns) {
	ignore_blockstart();
	wpl_text *text = new wpl_text();
	append_child(text);
	text->load_position(get_position());
	text->parse_value(ns);
	load_position(text->get_position());
}

/**
 * @brief Parse a parseable with unknown, but funky syntax. The parseable can add something to our namespace if it wants to.
 */
void wpl_block::parse_parseable(wpl_namespace *ns, wpl_parseable *parseable) {
	parseable->load_position(get_position());
	try {
		try {
			try {
				parseable->parse_value(ns);
			}
			catch (wpl_type_begin_variable_declaration &e) {
				e.create_variable(ns);
				load_position(e.get_position_at_name());
				parse_expression(ns);
			}
		}
		catch (wpl_type_begin_function_declaration &e) {
			e.load_position(parseable->get_position());
			e.parse_value(ns);
			load_position(e.get_position());
		}
	}
	catch (wpl_type_end_statement &e) {
		load_position(e.get_position());
		ignore_whitespace();
		if (!ignore_letter (';')) {
			THROW_ELEMENT_EXCEPTION("Expected ';' after declaration of incomplete type");
		}
	}
}

/**
 * @brief Parse a comment block ending with * / (without the space, can't write it here in C++ :-) )
 */
void wpl_block::parse_comment() {
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

/**
 * @brief Parse a block, ending with }
 *
 * @param Flags to limit which elements we allow depening on current context
 */
void wpl_block::parse_value(wpl_namespace *ns) {
#ifdef WPL_DEBUG_BLOCKS
	DBG("B: (" << this << "): Parse" << endl);
#endif
	wpl_matcher_position start_pos = get_position();

	char buf[WPL_VARNAME_SIZE+1];
	// Descend into block and find child blocks and elements
	while (!at_end()) {
		const char *prev_pos = get_string_pointer();

		ignore_string_match (WHITESPACE,0);

		if (at_end()) { break; }

		if (int len = search (WORD, 0, false)) {
			check_varname_length(len);
			get_string(buf, len);

			append_child_position();

			if (strcmp (buf, wpl_blockname_if) == 0) {
				parse_if_else_sequenze(ns);
			}
			else if (strcmp (buf, wpl_blockname_while) == 0) {
				parse_while(ns);
			}
			else if (strcmp (buf, wpl_blockname_text) == 0) {
				parse_text(ns);
			}
			else if (wpl_parseable *parseable = ns->new_find_parseable(buf)) {
				parse_parseable(ns, parseable);
			}
			else {
				revert_string(len);
				parse_expression(ns);
			}
		}
		else if (ignore_string ("/*")) {
			parse_comment();
		}
		else if (ignore_letter ('{')) {
			append_child_position();
			parse_block(ns);
		}
		else if (ignore_letter ('}')) {
			return;
		}
		else if (ignore_letter ('#')) {
			append_child_position();
			parse_pragma(ns);
		}
		else if (search (EXPRESSION, WHITESPACE, false)) {
			append_child_position();
			parse_expression(ns);
		}
		else {
			ostringstream msg;
			msg << "Syntax error in block near '" << get_string_pointer()[0] << "'";
			THROW_ELEMENT_EXCEPTION(msg.str());
		}
	}
	load_position(start_pos);
	revert_string(1);
	THROW_ELEMENT_EXCEPTION("Missing '}' after block, block began here:");
}

int wpl_block::run_children (wpl_block_state *block_state, wpl_value *final_result) {
	int ret = WPL_OP_NO_RETURN;

	int i = 0;
	for (unique_ptr<wpl_runable> &r : child_elements) {
		try {
			ret = block_state->run_child(r.get(), i, final_result);
		}
		catch (const runtime_error &e) {
			throw wpl_element_exception(e.what(), child_positions[i]);
		}
		if (ret & (WPL_OP_RETURN|WPL_OP_BREAK)) {
			break;
		}
		i++;
	}

	return ret;
}

bool wpl_block::check_run(wpl_block_state *block_state) {
	if (run_condition) {
#ifdef WPL_DEBUG_BLOCKS
		DBG("BI (" << (wpl_block*)this << "): Running condition " << run_condition << endl);
#endif
		wpl_value_bool return_value;
		block_state->run_run_condition(run_condition, &return_value);
		return return_value.get();
	}
	else {
		return true;
	}
}

int wpl_block::run(wpl_state *state, wpl_value *final_result) {
	wpl_block_state *block_state = (wpl_block_state*) state;

#ifdef WPL_DEBUG_BLOCKS
	DBG("B: (" << this << "): Run" << endl);
#endif

	return run_children(block_state, final_result);
}
/*
int wpl_block::run_no_variables(wpl_namespace_session &nss, wpl_value *final_result) {
#ifdef WPL_DEBUG_BLOCKS
	DBG("B: (" << this << "): Run w/o variables" << endl);
#endif
	wpl_namespace_session my_nss(&nss);
	my_nss.set_namespace(this);
	return run_children(my_nss, final_result);


int wpl_block::run_no_nss(wpl_namespace_session &nss, wpl_value *final_result) {
#ifdef WPL_DEBUG_BLOCKS
	DBG("B: (" << this << "): Run w/o namespace session" << endl);
#endif
	return run_children(nss, final_result);
}
*/
