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

#include "expression.h"
#include "expression_state.h"
#include "exception.h"
#include "blocks.h"
//#include "block.h"
#include "string.h"
#include "function.h"
#include "operator.h"
#include "wpl_string.h"
#include "debug.h"

#include "value.h"
#include "value_int.h"
#include "value_string.h"
#include "value_unresolved.h"
//#include "value_expression.h"
#include "value_wrapper.h"
#include "value_list.h"
#include "value_regex.h"

#include <typeinfo>
#include <iostream>
#include <cstdio>
#include <list>

using namespace std;

wpl_expression::~wpl_expression() {
}

wpl_state *wpl_expression::new_state(wpl_namespace_session *nss, wpl_io *io) {
	return new wpl_expression_state(nss, io, get_run_stack());
//	this->nss = nss;wpl_block_state *parent_block_state;
/*	if (!(parent_block_state = dynamic_cast<wpl_block_state*>(parent))) {
		cerr << "Parent pointer was " << parent << " of type " << typeid(parent).name() << endl;
		throw runtime_error("wpl_expression::new_state(): did not receive block state");
	}
	return new wpl_expression_state(parent_block_state, get_run_stack());*/
}

int wpl_expression::run (
		wpl_state *state,
		wpl_value *final_result
		)
{
	//dump();

	wpl_expression_state *exp_state = (wpl_expression_state*) state;
	exp_state->revert();

	shunting_yard_carrier first_carrier = exp_state->top();
	exp_state->pop();

	if (exp_state->empty() && !final_result) {
		return (WPL_OP_OK|WPL_OP_NO_RETURN);
	}

	if (first_carrier.op && first_carrier.value) {
		return first_carrier.value->do_fastop(exp_state, final_result, first_carrier.op);
	}

	if (first_carrier.op) {
		if (first_carrier.op == &OP_BREAK_OP) {
			return (WPL_OP_OK|WPL_OP_BREAK);
		}
		else if (first_carrier.op == &OP_RETURN_OP) {
			return (WPL_OP_OK|WPL_OP_RETURN|WPL_OP_NO_RETURN);
		}
		cerr << "Found lonely operator '" << first_carrier.op->name << "' in expression\n";
		throw runtime_error("Single operator in expression was not 'break' or 'return'");
	}
	else if (first_carrier.value) {
		return first_carrier.value->do_operator_recursive (
				exp_state,
				final_result
		);
	}
	else {
		throw runtime_error("wpl_expression::run(): Nothing set for first carrier");
	}
}

void wpl_expression::add_constant (wpl_value *value) {
	constant_values.push_back(unique_ptr<wpl_value>(value));
	value->set_flags(wpl_value_is_constant);
}

const struct wpl_operator_struct *wpl_expression::find_operator(int flags) {
	char tmp[WPL_OP_NAME_MAX_LENGTH+1];
	const struct wpl_operator_struct *ret = NULL;

	int len = search (NON_WHITESPACE, 0, false);
	const char *pos = get_string_pointer();
	int maxlen = std::min<int>(len,WPL_OP_NAME_MAX_LENGTH);

	strncpy(tmp, pos, maxlen);

	for (int i = maxlen; i > 0; i--) {
		tmp[i] = '\0';
		if (ret = wpl_get_operator(tmp, flags)) {
			break;
		}
	}

	return ret;
}

void wpl_expression::parse_number(const char *text) {
#ifdef WPL_DEBUG_EXPRESSIONS
	DBG("EXP (" << this << "): Parsing number '" << text << "'\n");
#endif

	// TODO validate numbe
	wpl_value_string tmp(text);
	wpl_value *value = new wpl_value_int(tmp.toInt());
	add_constant(value);

	if (value == NULL) {
		throw runtime_error("wpl_expression::parse_number(const char*,int): wpl_value_factory returned null");
	}

	shunt(value);

	expect &= ~(EXPECT_NUMBER);
	expect |= EXPECT_OPERATOR;
}

void wpl_expression::parse_par_open() {
	expect &= ~(EXPECT_OPERATOR | EXPECT_OPEN_PAR);
	expect |= EXPECT_NUMBER;

	par_level++;

	shunt_operator(&OP_PAR_OPEN);
}

void wpl_expression::parse_par_close() {
	const struct wpl_operator_struct *op;
	if (--par_level < 0) {
		THROW_ELEMENT_EXCEPTION("Mismatched ')' in expression");
	}

	if ((expect & EXPECT_END_ON_PAR) && (par_level == 0)) {
		expect |= EXPECT_DO_BREAK;
	}
	else {
		expect &= ~(EXPECT_NUMBER);
		expect |= EXPECT_OPERATOR;
	}

	shunt_operator(&OP_PAR_CLOSE);
}

void wpl_expression::parse_operator(const struct wpl_operator_struct *op) {
	ignore_string(op->length);

#ifdef WPL_DEBUG_EXPRESSIONS
	DBG("EXP (" << this << "): Add operator " << op->name << endl);
#endif

	shunt_operator(op);

	expect &= ~(EXPECT_OPERATOR);
	expect |= EXPECT_NUMBER;
}

void wpl_expression::parse_semicolon() {
	if (expect & EXPECT_END_ON_PAR) {
		// TODO implement for-loop
	}
	else if (par_level == 0) {
		expect |= EXPECT_DO_BREAK;
	}
	else {
		cerr << "Parantheses level is : " << par_level << endl;
		THROW_ELEMENT_EXCEPTION("Unexpected ; in expression")
	}
}

#define LOAD_POSITION_AND_PARSE(element) \
	element->load_position(get_static_position()); \
	element->parse_value(parent_namespace); \
	load_position(element->get_static_position())
/*
void wpl_expression::parse_block(wpl_namespace *parent_namespace) {
	wpl_block *block = new wpl_block();
	add_constant(block);

	block->set_parent_namespace(parent_namespace);
	block->load_position(get_static_position());
	block->parse_value();
	load_position(block->get_static_position());

	shunt(block);

	expect &= ~(EXPECT_NUMBER);
	expect |= EXPECT_OPERATOR;
}
*/
void wpl_expression::parse_string(wpl_namespace *parent_namespace) {
	char quote = get_letter (QUOTE);

	bool double_escape;
	if (M_SINGLE_QUOTE(quote)) {
		double_escape = false;
	}
	else if (M_DOUBLE_QUOTE(quote)) {
		double_escape = true;
	}
	else {
		THROW_ELEMENT_EXCEPTION("Unknown quote");
	}

	int element_count = 1;
	string original_string;
	original_string.reserve(50);
	while (true) {
		if (char letter = get_letter(NONCTRL_ASCII|UTF8, 0)) {
			if (letter == quote) {
				if (element_count > 1) {
					shunt_operator(&OP_CONCAT);
				}
				wpl_value_string *val = new wpl_value_string(original_string.c_str());
				add_constant (val);
				shunt(val);
				break;
			}
			else if (letter == '\\') {
				char tmp;
				char letter = get_letter (ALL);
				if (double_escape) {
					if (!wpl_string_parse_double_escape (&tmp, letter)) {
						THROW_ELEMENT_EXCEPTION("Unknown double escape sequence");
					}
					original_string += tmp;
				}
				else {
					if (wpl_string_parse_single_escape (&tmp, letter)) {
						original_string += tmp;
					}
					else {
						original_string += '\\';
						original_string += letter;
					}
				}
			}
			else if (double_escape && (letter == '$')) {
				char name[WPL_VARNAME_SIZE];
			       
				wpl_value_string *val = new wpl_value_string(original_string.c_str());
				add_constant (val);

				int len = search (WORD, 0, false);
				check_varname_length(len);
				get_string(name, len);
				
				if (element_count > 1) {
					shunt_operator(&OP_CONCAT);
				}
				shunt(val);
				shunt_operator(&OP_CONCAT);
				parse_unresolved_identifier(parent_namespace, name);

				original_string = "";
				element_count++;
			}
/*			else if (double_escape && (letter == '{')) {
				wpl_value_string *val = new wpl_value_string(original_string.c_str());
				add_constant (val);

				wpl_expression *exp = new wpl_expression_loose_end();
				wpl_value_expression *exp_value = new wpl_value_expression(exp);
				add_constant (exp_value);

				LOAD_POSITION_AND_PARSE(exp);

				if (!ignore_letter('}')) {
					THROW_ELEMENT_EXCEPTION("Expected '}' after inline expression");
				}

				if (element_count > 1) {
					shunt_operator(&OP_CONCAT);
				}
				shunt(val);
				shunt_operator(&OP_CONCAT);
				shunt(exp_value);

				original_string = "";
				element_count++;
			}*/
			else {
				original_string += letter;
			}
		}
		else {
			THROW_ELEMENT_EXCEPTION("Unknown character in quoted string");
		}
	}

	expect &= ~(EXPECT_NUMBER);
	expect |= EXPECT_OPERATOR;
}

void wpl_expression::parse_function_call (
		wpl_namespace *parent_namespace,
		wpl_value_unresolved_identifier *ui
		)
{
	uint32_t expect_save = expect;
	int par_level_save = par_level;

	shunt(ui);

	prepare_operator(&OP_FUNCTION_CALL);
	parse (parent_namespace, (EXPECT_OPEN_PAR|EXPECT_END_ON_PAR|EXPECT_NUMBER));
	shunt_operator(&OP_FUNCTION_CALL);

/*	wpl_expression_par_enclosed *function_arguments = 
		new wpl_expression_par_enclosed();
	wpl_value_expression *exp_value = new wpl_value_expression(function_arguments);
	add_constant(exp_value);

	LOAD_POSITION_AND_PARSE(function_arguments);
	function_arguments->shunt_operator(&OP_FUNCTION_CALL);
	function_arguments->shunt(ui);
	function_arguments->finish();

	shunt(exp_value);*/

	par_level = par_level_save;
	expect = expect_save;
}

void wpl_expression::parse_unresolved_identifier(wpl_namespace *parent_namespace, const char *text) {
#ifdef WPL_DEBUG_EXPRESSIONS
	DBG("EXP (" << this << "): Identifier '" << text << "' remains unresolved at parse time\n");
#endif
	wpl_value_unresolved_identifier *ui = new wpl_value_unresolved_identifier(text);
	add_constant(ui);
	ignore_string_match(WHITESPACE,0);
	if (search_letter ('(')) {
		parse_function_call(parent_namespace, ui);
	}
	else {
		shunt(ui);
	}

	expect &= ~(EXPECT_NUMBER);
	expect |= EXPECT_OPERATOR;
}

void wpl_expression::parse_regex(const char *prefix) {
	wpl_matcher_position start;
	save_position(&start);

	char letter;
	string regex_string;
	regex_string.reserve(50);

	while (letter = get_letter()) {
		if (letter == '\\') {
			regex_string += letter;
			regex_string += get_letter();
		}
		else if (letter == '/') {
			break;
		}
		else {
			regex_string += letter;
		}
	}

	if (at_end()) {
		load_position(&start);
		THROW_ELEMENT_EXCEPTION("Non-terminated regular expression runs out of file");
	}

	wpl_value_regex *regex = new wpl_value_regex(regex_string.c_str());
	add_constant(regex);
	shunt(regex);

	expect &= ~(EXPECT_NUMBER);
	expect |= EXPECT_OPERATOR;
}

void wpl_expression::parse(wpl_namespace *parent_namespace, uint32_t _expect) {
	expect = _expect;

	par_level = 0;
	if (expect == 0) {
		expect |= EXPECT_NUMBER;
	}

	
	while (!at_end() && !(expect & EXPECT_DO_BREAK)) {
		int whitespace_length = ignore_string_match (WHITESPACE, 0);

		if (at_end()) {
			THROW_ELEMENT_EXCEPTION("Unexpected EOF in expression");
		}

		int operator_search_flags = (
			(expect & EXPECT_NUMBER) ? WPL_OP_F_ASSOC_RIGHT : WPL_OP_F_ASSOC_ALL
		);

		if (ignore_letter ('(')) {
			parse_par_open();
		}
		else if (ignore_letter (')')) {
			parse_par_close();
		}
		else if (expect & EXPECT_OPEN_PAR) {
			THROW_ELEMENT_EXCEPTION("Syntax error: Expected '('");
		}
		else if (const struct wpl_operator_struct *op = find_operator(operator_search_flags)) {
			parse_operator (op);
		}
		else if (int len = search (NUMBER, 0, false)) {
			check_varname_length(len);
			get_string(buf, len);
			parse_number(buf);
		}
		else if (int len = search (WORD, 0, false)) {
			check_varname_length(len);
			get_string(buf, len);
			if (ignore_letter('/')) {
				parse_regex(buf);
			}
			else {
				parse_unresolved_identifier(parent_namespace, buf);
			}
		}
		else if (search (QUOTE, 0, false)) {
			parse_string(parent_namespace);
		}
/*		else if (ignore_letter ('{')) {
			parse_block(parent_namespace);
		}*/
		else if (ignore_letter ('/')) {
			parse_regex("");
		}
		else if (ignore_letter (';')) {
			parse_semicolon();
		}
		else if (ignore_letter ('[')) {
			parse_par_open();
		}
		else if (ignore_letter (']')) {
			shunt_operator(&OP_ARRAY_SUBSCRIPTING);
			parse_par_close();
		}
		else if (expect & EXPECT_LOOSE_END) {
			revert_string(whitespace_length);
			break;
		}
		else {
			ostringstream msg;
			msg << "Syntax error in expression near '" << get_string_pointer()[0] << "'";
			THROW_ELEMENT_EXCEPTION(msg.str());
		}
	}
}

void wpl_expression::parse_value(wpl_namespace *parent_namespace) {
#ifdef WPL_DEBUG_EXPRESSIONS
	DBG("EX (" << this << "): Parsing expression\n");
#endif
	parse(parent_namespace, 0);
	finish();
}
