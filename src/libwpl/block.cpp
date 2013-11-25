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

#include <utility>
#include <cstring>
#include <cstdio>

using namespace std;

wpl_block::wpl_block() {
	run_condition = NULL;
}

wpl_block::~wpl_block() {
#ifdef WPL_DEBUG_DESTRUCTION
	DBG("B (" << this << "): Destructing wpl_block\n");
#endif
	if (run_condition) {
		delete run_condition;
	}
}

wpl_state *wpl_block::new_state(wpl_namespace_session *nss) {
	return new wpl_block_state (nss, this);
}

void wpl_block::append_child (wpl_runable *element) {
	child_elements.push_back(unique_ptr<wpl_runable>(element));
}

void wpl_block::parse_expression (wpl_namespace *ns) {
	wpl_expression *exp = new wpl_expression();

	append_child(exp);

	exp->load_position(get_static_position());
	exp->parse_value(ns);
	load_position(exp->get_static_position());
}

void wpl_block::parse_block (wpl_namespace *ns) {
	wpl_block *block = new wpl_block();
	append_child (block);

	block->set_parent_namespace(ns);
	block->load_position(get_static_position());
	block->parse_value(block);
	load_position(block->get_static_position());
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

	pragma->load_position(get_static_position());
	pragma->parse_value(ns);
	load_position(pragma->get_static_position());
}

void wpl_block::parse_while(wpl_namespace *ns) {
	wpl_block_while *block = new wpl_block_while();
	append_child(block);

	wpl_expression *exp = new wpl_expression_par_enclosed();
	block->set_run_condition(exp);

	exp->load_position(get_static_position());
	exp->parse_value(ns);
	load_position(exp->get_static_position());

	ignore_blockstart();

	block->set_parent_namespace(ns);
	block->load_position(get_static_position());
	block->parse_value(block);
	load_position(block->get_static_position());
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

	exp->load_position(get_static_position());
	exp->parse_value(ns);
	load_position(exp->get_static_position());

	ignore_blockstart();

	block->set_parent_namespace(ns);
	block->load_position(get_static_position());
	block->parse_value(block);
	load_position(block->get_static_position());

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
		block_else_if->load_position(get_static_position());
		block_else_if->parse_value(block_else_if);
		load_position(block_else_if->get_static_position());

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
		block_else->load_position(get_static_position());
		block_else->parse_value(block_else);
		load_position(block_else->get_static_position());
	}
}

/**
 * @brief Parse a variable declaration like in a function like 'int a'
 *
 * @param Pointer to the type object (int, array etc.)
 */
void wpl_block::parse_function_argument (wpl_namespace *ns) {
	char buf[WPL_VARNAME_SIZE];
	get_word (buf);

	if (wpl_type_complete *type = ns->find_complete_type(buf)) {
		parse_complete_type(ns, type, true);
	}
	else if (wpl_type_template *type = ns->find_template_type(buf)) {
		parse_template_type(ns, type, true);
	}
	else {
		cerr << "While parsing name '" << buf << "':\n";
		THROW_ELEMENT_EXCEPTION("wpl_block::parse_function_argument(): Unknown complete or template type");
	}
}

/**
 * @brief Parse this block as a function argument block like '(int a, int b, string c)'
 */
void wpl_block::parse_function_arguments(wpl_namespace *ns) {
	ignore_string_match(WHITESPACE, 0);

	if (ignore_letter (')')) {
		// No arguments
		return;
	}

	do {
		ignore_string_match(WHITESPACE, 0);
		parse_function_argument(ns);
		ignore_string_match(WHITESPACE, 0);

		if (ignore_letter (',')) {
			continue;
		}
		else if (ignore_letter (')')) {
			return;
		}
		else {
			snprintf (exception_msg, exception_msg_length,
				"Syntax error in function declaration near '%c'", get_string_pointer()[0]);
			THROW_ELEMENT_EXCEPTION(exception_msg);
		}
	} while (!at_end());

	THROW_ELEMENT_EXCEPTION("Expected ')' after function argument declaration");
}

void wpl_block::parse_function_declaration (
	wpl_namespace *ns,
	const wpl_type_complete *return_type,
	const char *name
) {
	wpl_user_function *function = new wpl_user_function(name, return_type);
	ns->register_identifier(function);

	function->load_position(get_static_position());
	function->parse_value(ns);
	load_position(function->get_static_position());
}

/**
 * @brief Parse a template specification for template types in a declaration
 *
 * @param type Pointer to the template type
 */
void wpl_block::parse_template_type (
	wpl_namespace *ns,
	const wpl_type_template *type,
	bool function_ctx
) {
	char buf[WPL_VARNAME_SIZE];

	ignore_string_match (WHITESPACE, 0);

	if (!ignore_letter ('<')) {
		THROW_ELEMENT_EXCEPTION("Expected '<' after declaration of template type");
	}

	get_word (buf);

	if (wpl_type_complete *subtype = ns->find_complete_type(buf)) {
		unique_ptr<wpl_type_complete> new_type (
			type->new_instance(subtype)
			);

		const wpl_type_complete *final_type;
		if (!(final_type = ns->find_complete_type(new_type->get_name()))) {
			ns->register_identifier(new_type.get());
			final_type = new_type.release();
		}

		ignore_string_match (WHITESPACE, 0);
		if (!ignore_letter ('>')) {
			THROW_ELEMENT_EXCEPTION("Expected '>' after template specification");
		}

		parse_complete_type(ns, final_type, function_ctx);
	}
	else if (wpl_type_template *subtype = ns->find_template_type(buf)) {
		parse_template_type(ns, subtype, function_ctx);

		ignore_string_match (WHITESPACE, 0);
		if (!ignore_letter ('>')) {
			THROW_ELEMENT_EXCEPTION("Expected '>' after template specification");
		}
	}
	else {
		THROW_ELEMENT_EXCEPTION("Could not find complete type from template specification");
	}
}

/**
 * @brief Parse a complete type like 'int a', possibly followed by a function argument declaration '('
 *
 * @param type Pointer to the complete type
 * @param name Name of the new variable
 */
void wpl_block::parse_complete_type (
	wpl_namespace *ns,
	const wpl_type_complete *type,
	bool function_ctx
) {
	struct wpl_matcher_position position_at_name;
	save_position (&position_at_name);

	char buf[WPL_VARNAME_SIZE];
	get_word(buf);

	ignore_string_match(WHITESPACE, 0);

	if (ignore_letter('(')) {
		if (function_ctx) {
			THROW_ELEMENT_EXCEPTION("No function declaration allowed inside function argument list");
		}
		parse_function_declaration(ns, type, buf);
	}
	else {
		wpl_variable_holder variable(type->new_instance(), buf, WPL_VARIABLE_ACCESS_PRIVATE);
		ns->register_identifier(&variable);
		if (!function_ctx) {
			load_position (&position_at_name);
		}
	}
}

/**
 * @brief Parse and incomplete type declaration, like struct
 *
 * @param type Pointer to the incomplete type
 * @param name Name of the new type created by the incomplete
 */
void wpl_block::parse_incomplete_type (
	wpl_namespace *ns,
	const wpl_type_incomplete *type
) {
	char buf[WPL_VARNAME_SIZE];
	get_word(buf);

	wpl_type_user_incomplete *usr_obj = type->new_instance(buf);
	register_identifier(usr_obj);

	usr_obj->set_parent_namespace(this);
	usr_obj->load_position(get_static_position());
	usr_obj->parse_value(usr_obj);
	load_position(usr_obj->get_static_position());
}

/**
 * @brief Parse a TEXT{}-block
 */
void wpl_block::parse_text(wpl_namespace *ns) {
	ignore_blockstart();
	wpl_text *text = new wpl_text();
	append_child(text);
	text->load_position(get_static_position());
	text->parse_value(ns);
	load_position(text->get_static_position());
}

/**
 * @brief Parse a parseable with unknown, but funky syntax. The parseable can add something to our namespace if it wants to.
 */
void wpl_block::parse_parseable(wpl_namespace *ns, wpl_parseable *parseable) {
	parseable->load_position(get_static_position());
	parseable->parse_value(ns);
	load_position(parseable->get_static_position());
}

/**
 * @brief Parse a comment block ending with * / (without the space, can't write it here in C++ :-) )
 */
void wpl_block::parse_comment() {
	wpl_matcher_position start;
	save_position(&start);

	while (get_letter ('*', NON_ASTERISK)) {
		if (ignore_letter ('/')) {
			return;
		}
	}

	load_position(&start);
	revert_string(2);

	THROW_ELEMENT_EXCEPTION("Could not find comment end for this comment");
}

/**
 * @brief Parse a block, ending with }
 *
 * @param Flags to limit which elements we allow depening on current context
 */
void wpl_block::__parse_value(wpl_namespace *ns, int flags) {
#ifdef WPL_DEBUG_BLOCKS
	DBG("B: (" << this << "): Parse" << endl);
#endif
	wpl_matcher_position start_pos;
	save_position(&start_pos);

	char buf[WPL_VARNAME_SIZE+1];
	// Descend into block and find child blocks and elements
	while (!at_end()) {
		const char *prev_pos = get_string_pointer();

		ignore_string_match (WHITESPACE,0);

		if (at_end()) { break; }

		if (int len = search (WORD, 0, false)) {
			check_varname_length(len);
			get_string(buf, len);

			if (strcmp (buf, wpl_blockname_if) == 0) {
				if (flags & parse_f_only_declarations) {
					THROW_ELEMENT_EXCEPTION("'if' not allowed here");
				}
				parse_if_else_sequenze(ns);
				continue;
			}
			else if (strcmp (buf, wpl_blockname_while) == 0) {
				if (flags & parse_f_only_declarations) {
					THROW_ELEMENT_EXCEPTION("'while' not allowed here");
				}
				parse_while(ns);
				continue;
			}
			else if (strcmp (buf, wpl_blockname_text) == 0) {
				if (flags & parse_f_only_declarations) {
					THROW_ELEMENT_EXCEPTION("'TEXT' not allowed here");
				}
				parse_text(ns);
				continue;
			}
			else if (const wpl_type_complete *type = ns->find_complete_type(buf)) {
				parse_complete_type(ns, type, false);
				continue;
			}
			else if (const wpl_type_template *type = ns->find_template_type(buf)) {
				parse_template_type(ns, type, false);
				continue;
			}
			else if (const wpl_type_incomplete *type = ns->find_incomplete_type(buf)) {
				parse_incomplete_type(ns, type);
				continue;
			}
			else if (wpl_parseable *parseable = ns->find_parseable(buf)) {
				parse_parseable(ns, parseable);
				continue;
			}
			else if (wpl_variable *variable = ns->find_variable(buf)) {
			}
			else if (wpl_function *function= ns->find_function(buf)) {
			}
			else if (wpl_function *function= ns->find_function(buf)) {
			}
			else if (wpl_get_operator(buf, WPL_OP_F_ASSOC_ALL)) {
			}
			else {
				revert_string(len);
				cerr << "While parsing name '" << buf << "':\n";
				THROW_ELEMENT_EXCEPTION("Unresolved name");
			}

			if (!(flags & parse_f_only_declarations)) {
				revert_string(len);
			}
			else {
				if (!ignore_letter (';')) {
					THROW_ELEMENT_EXCEPTION("Excepted ';' after declaration");
				}
				continue;
			}	
		}

		ignore_string_match (WHITESPACE,0);

		if (ignore_string ("/*")) {
			parse_comment();
		}
		else if (ignore_letter ('{')) {
			if (flags & parse_f_only_declarations) {
				THROW_ELEMENT_EXCEPTION("'{' or blockstart not allowed here");
			}
			parse_block(ns);
		}
		else if (ignore_letter ('}')) {
			return;
		}
		else if (search (EXPRESSION, WHITESPACE, false)) {
			if (flags & parse_f_only_declarations) {
				THROW_ELEMENT_EXCEPTION("Expression not allowed here");
			}
			parse_expression(ns);
		}
		else if (ignore_letter ('#')) {
			if (flags & parse_f_only_declarations) {
				THROW_ELEMENT_EXCEPTION("'#' or pragma not allowed here");
			}
			parse_pragma(ns);
			continue;
		}

		if (prev_pos == get_string_pointer()) {
			snprintf (exception_msg, exception_msg_length,
				"Syntax error in block near '%c'", get_string_pointer()[0]);
			THROW_ELEMENT_EXCEPTION(exception_msg);
		}
	}
	load_position(&start_pos);
	revert_string(1);
	THROW_ELEMENT_EXCEPTION("Missing '}' after block, block began here:");
}

int wpl_block::run_children (wpl_block_state *block_state, wpl_value *final_result) {
	int ret = WPL_OP_NO_RETURN;

	int i = 0;
	for (unique_ptr<wpl_runable> &r : child_elements) {
		ret = block_state->run_child(r.get(), i, final_result);
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
