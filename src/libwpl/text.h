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
#include "parseable.h"
#include "expression.h"
#include "text_state.h"
#include "namespace_session.h"
#include "io.h"

#include <set>
#include <deque>

class chunk;
class wpl_value;

class wpl_text : public wpl_runable, public wpl_parseable {
	protected:
	class chunk {
		private:
		const char *start;
		const char *end;
		unique_ptr<wpl_expression> expression;
		unique_ptr<wpl_text> loop_text;

		public:
		chunk (const char *start, const char *end);
		chunk (wpl_expression *expression);
		chunk (wpl_text *text, wpl_expression *expression);
		int run(wpl_text_state *state, int index, wpl_value *final_result, wpl_io &io);

		chunk(const chunk &copy) {
			throw runtime_error("Cannot clone text chunk");
		}
		const char *get_start() const {
			return start;
		}
		const char *get_end()const {
			return end;
		}
		int length() {
			return end-start;
		}
		wpl_expression *get_expression() const {
			return expression.get();
		}
		wpl_text *get_loop_text() const {
			return loop_text.get();
		}
	};

	private:
	chunk *push_chunk(const char *start, const char *end);

	protected:
	deque<chunk> chunks;

	public:
	wpl_text (const char *name) : wpl_parseable(name) {};
	wpl_text () : wpl_parseable("TEXT") {};
	virtual void suicide() override {
		delete this;
	}
	virtual ~wpl_text() {}

	wpl_state *new_state(wpl_namespace_session *nss, wpl_io *io) {
		return new wpl_text_state(nss, io, chunks.size());
	}

	virtual void parse_value(wpl_namespace *parent_namespace);
	int run(wpl_state *state, wpl_value *final_result, wpl_io &io);
	virtual int run(wpl_state *state, wpl_value *final_result);
	int output_json(wpl_state *state, const set<wpl_value*> &vars, wpl_value *final_result);
	int output_as_json_var(wpl_state *state, wpl_value *final_result);
};

class wpl_text_var_io_method : public wpl_runable {
	private:
	wpl_text *my_text;

	public:
	wpl_text_var_io_method(wpl_text *my_text) {
		this->my_text = my_text;
	}
	wpl_state *new_state(wpl_namespace_session *nss, wpl_io *io) {
		return my_text->new_state(nss, io);
	}
	int run (wpl_state *state, wpl_value *final_result) {
		return my_text->output_as_json_var(state, final_result);
	}
};
