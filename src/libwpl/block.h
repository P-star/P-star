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

#pragma once

#include "runable.h"
#include "namespace.h"

#include <stdint.h>

class wpl_state;
class wpl_expression;
class wpl_block_state;
class wpl_type_complete;
class wpl_type_complete;
class wpl_type_incomplete;
class wpl_type_template;
class wpl_type;

using namespace std;

class wpl_block : public wpl_runable, public wpl_namespace, public wpl_matcher {
	private:
	int parse_statement_to_semicolon (const char **target, uint32_t match, uint32_t ignore);
	list <unique_ptr<wpl_runable>> child_elements;
	vector<struct wpl_matcher_position> child_positions;

	void append_child (wpl_runable *element);
	void append_child_position ();

	template<typename T> void parse_element_set_namespace(T *obj);
	template<typename T> void parse_element_send_namespace(T *obj);

	void parse_expression (wpl_namespace *ns);
	void parse_block (wpl_namespace *ns);
	void parse_pragma (wpl_namespace *ns);

	void parse_for(wpl_namespace *ns);
	void parse_while(wpl_namespace *ns);
	void parse_if_else_sequenze(wpl_namespace *ns);

	void parse_text(wpl_namespace *ns);
	void parse_parseable(wpl_namespace *ns, wpl_parseable *parseable);
	void parse_comment();

	static const int parse_f_none			= 0;
	static const int parse_f_only_declarations	= 1<<0;

	int run_children(wpl_block_state *block_state, wpl_value *final_result);

	protected:
	bool check_run(wpl_block_state *block_state);
	wpl_expression *run_condition;

	public:
	wpl_block();
	virtual ~wpl_block();

	wpl_state *new_state(wpl_namespace_session *nss, wpl_io *io) override;

	int run (wpl_state *state, wpl_value *final_result);

	void parse_value(wpl_namespace *ns);

	void set_run_condition(wpl_expression *exp) {
		run_condition = exp;
	}
};

