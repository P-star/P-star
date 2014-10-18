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

#include "expression_state.h"
#include "runable.h"
#include "matcher.h"
#include "shunting_yard.h"
#include "identifier.h"
#include "namespace.h"
#include "namespace_session.h"
#include "value.h"

#include <iostream>
#include <stdint.h>
#include <list>
#include <stack>
#include <memory>

using namespace std;

typedef struct wpl_operator_struct wpl_operator_struct;
class wpl_value_unresolved_identifier;
class wpl_value;

class wpl_expression : public wpl_runable, public shunting_yard, public wpl_matcher  {
	private:
	list<unique_ptr<wpl_value>> constant_values;

	protected:
	void add_constant(wpl_value *value);
	int __run(wpl_namespace_session &nss, wpl_value *final_result, wpl_expression_state &exp_state);
	
	// Parsing
	int par_level;
	uint32_t expect;
	char buf[WPL_VARNAME_SIZE];

	const struct wpl_operator_struct *find_operator(int flags);

	void parse_number(const char *text);
	void parse_par_open();
	void parse_par_close();
	void parse_operator(const struct wpl_operator_struct *op);
	void parse_semicolon();

	/*
	   Block parsing is disabled so that a block only has to
	   be runable and not shuntable
	   */
//	void parse_block(wpl_namespace *parent_namespace);
	void parse_string(wpl_namespace *parent_namespace);
	void parse_array_subscripting(wpl_namespace *parent_namespace, wpl_value_unresolved_identifier *ui);
	void parse_function_call(wpl_namespace *parent_namespace, wpl_value_unresolved_identifier *ui);
	void parse_unresolved_identifier(wpl_namespace *parent_namespace, const char *text);
	void parse_quoted_string(wpl_namespace *parent_namespace);
	void parse_regex(wpl_namespace *parent_namespace);

	void parse (wpl_namespace *parent_namespace);

	public:
	static const uint32_t EXPECT_ALL =		0;
	static const uint32_t EXPECT_OPERATOR =		1<<0;
	static const uint32_t EXPECT_NUMBER =		1<<1;

	static const uint32_t EXPECT_IS_FIRST =		1<<5;

	static const uint32_t EXPECT_DO_BREAK =		1<<10;
	static const uint32_t EXPECT_SEMICOLON_END =	1<<11;

	// End softly instead of getting syntax error at unknown chars
	static const uint32_t EXPECT_LOOSE_END =	1<<29;

	/*
	   These are used to parse block-statements like
	   "if (EXPRESSION) {", where we force the open par
	   and stop parsing and the last one. The if-keyword
	   should already be parsed.
	 */
	static const uint32_t EXPECT_OPEN_PAR =		1<<30;
	static const uint32_t EXPECT_END_ON_PAR =	1<<31;

	wpl_expression() {
		par_level = 0;
		this->expect = EXPECT_NUMBER|EXPECT_OPERATOR;
	}
	wpl_expression(uint32_t expect) {
		par_level = 0;
		this->expect = expect;
	}
	virtual ~wpl_expression();

	wpl_state *new_state(wpl_state *parent, wpl_namespace_session *nss, wpl_io *io) override;

	int run(wpl_state *exp_state, wpl_value *final_result) override;
	int run(wpl_state *exp_state, wpl_value *final_result, int loop_number) override;

	virtual void parse_value(wpl_namespace *parent_namespace);
	void parse_value (wpl_namespace *parent_namespace, uint32_t expect);
};

class wpl_expression_loose_end : public wpl_expression {
	public:

	virtual ~wpl_expression_loose_end() {}
	wpl_expression_loose_end() :
		wpl_expression(EXPECT_LOOSE_END|EXPECT_NUMBER)
	{}
	void parse_value(wpl_namespace *parent_namespace) override {
		parse(parent_namespace);
		finish();
	}
};

class wpl_expression_par_enclosed : public wpl_expression {
	public:

	virtual ~wpl_expression_par_enclosed() {}
	wpl_expression_par_enclosed() :
		wpl_expression(EXPECT_OPEN_PAR|EXPECT_END_ON_PAR|EXPECT_NUMBER)
	{}
	void insert_fake_open_par();
	void parse_value(wpl_namespace *parent_namespace) override {
		parse(parent_namespace);
		finish();
	}
};

class wpl_expression_quoted_string : public wpl_expression {
	private:
	public:
	void parse_value(wpl_namespace *parent_namespace) override {
		parse_string(parent_namespace);
	}
};

