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

#include "block_parser.h"

#include <vector>

class wpl_io;
class wpl_value;
class wpl_state;
class wpl_namespace;
class wpl_block_state;
class wpl_namespace_session;

class wpl_block : public wpl_block_parser {
	private:
	vector<struct wpl_matcher_position> child_positions;
	vector<unique_ptr<wpl_runable>> child_elements;

	void append_child (unique_ptr<wpl_runable> element);
	void append_child_position ();

	int run_children(wpl_block_state *block_state, wpl_value *final_result);

	public:
	wpl_block();
	wpl_block(const wpl_block &copy) :
		wpl_block_parser(copy),
		child_positions(),
		child_elements()
	{}
	virtual ~wpl_block() {}

	virtual void suicide() override {
		delete this;
	}

	wpl_block *new_instance() const override {
		return new wpl_block(*this);
	}

	wpl_state *new_state(wpl_state *parent, wpl_namespace_session *nss, wpl_io *io) override;
	int run (wpl_state *state, wpl_value *final_result) override;
	void parse_value(wpl_namespace *ns);
};
