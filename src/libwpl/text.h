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

#include "runable.h"
#include "parseable.h"
#include "expression.h"
#include "text_state.h"
#include "namespace_session.h"
#include "io.h"

#include <set>
#include <deque>

class wpl_value;

class wpl_text_chunk_end_reached {};
class wpl_text_chunk_it;

namespace wpl_text_chunks {
	class base {
		private:

		public:
		virtual ~base() {}
		virtual bool isText() {
			return false;
		}
		virtual int run (
			wpl_text_state *state,
			int index,
			wpl_value *final_result,
			wpl_io &io
		) = 0;
		virtual int output_json (
			wpl_text_state *state,
			const set<wpl_value*> &vars,
			wpl_text_chunk_it *it,
			wpl_value *final_result
		) = 0;
		virtual int run_raw (
			wpl_text_state *state,
			int index,
			wpl_value *final_result
		) {
			return WPL_OP_NO_RETURN;
		}
	};

	class text : public base {
		private:
		const char *start;
		const char *end;

		public:
		text (const char *start, const char *end) :
			start(start),
			end(end)
		{}
		virtual ~text() {}
		bool isText() override {
			return true;
		}
		const char *get_start() const {
			return start;
		}
		const char *get_end() const {
			return end;
		}
		int length() const {
			return end-start;
		}

		int run(wpl_text_state *state, int index, wpl_value *final_result, wpl_io &io) override;
		virtual int output_json (
			wpl_text_state *state,
			const set<wpl_value*> &vars,
			wpl_text_chunk_it *it,
			wpl_value *final_result
		);
	};

	class html_template : public base {
		private:
		wpl_template *my_template;

		public:
		html_template (wpl_template *my_template) :
			my_template(my_template)
		{}
		virtual ~html_template() {}

		int run (wpl_text_state *state, int index, wpl_value *final_result, wpl_io &io) override;
		int output_json (
			wpl_text_state *state,
			const set<wpl_value*> &vars,
			wpl_text_chunk_it *it,
			wpl_value *final_result
		);
	};

	class textblock : public base {
		private:
		unique_ptr<wpl_text> text;

		protected:
		wpl_text *get_text() const {
			return text.get();
		}

		public:
		textblock(wpl_text *text) :
			text(text)
		{}
		virtual ~textblock() {}

		int run(wpl_text_state *state, int index, wpl_value *final_result, wpl_io &io) override;
		virtual int output_json (
			wpl_text_state *state,
			const set<wpl_value*> &vars,
			wpl_text_chunk_it *it,
			wpl_value *final_result
		);
	};

	class expression : public base {
		private:
		unique_ptr<wpl_expression> exp;

		public:
		expression (wpl_expression *exp) :
			exp(exp)
		{}
		virtual ~expression() {}

		int run (wpl_text_state *state, int index, wpl_value *final_result, wpl_io &io) override;
		int run_raw (wpl_text_state *state, int index, wpl_value *final_result) override;
		virtual int output_json (
			wpl_text_state *state,
			const set<wpl_value*> &vars,
			wpl_text_chunk_it *it,
			wpl_value *final_result
		);
	};

	class exp_block : public textblock {
		private:
		unique_ptr<wpl_expression> exp;

		public:
		exp_block (wpl_text *text, wpl_expression *exp) :
			textblock(text),
			exp(exp)
		{}
		virtual ~exp_block() {}

		bool check_condition (wpl_text_state *state, int index);
	};

	class loop : public exp_block {
		public:
		loop (wpl_text *text, wpl_expression *exp) :
			exp_block(text, exp)
		{}
		virtual ~loop() {}

		int run (wpl_text_state *state, int index, wpl_value *final_result, wpl_io &io) override;
		int output_json (
			wpl_text_state *state,
			const set<wpl_value*> &vars,
			wpl_text_chunk_it *it,
			wpl_value *final_result
		);
	};

	class condition : public exp_block {
		private:
		public:
		condition (wpl_text *text, wpl_expression *exp) :
			exp_block(text, exp)
		{}
		virtual ~condition() {}

		int run (wpl_text_state *state, int index, wpl_value *final_result, wpl_io &io) override;
		int output_json (
			wpl_text_state *state,
			const set<wpl_value*> &vars,
			wpl_text_chunk_it *it,
			wpl_value *final_result
		);
	};
};

typedef deque<unique_ptr<wpl_text_chunks::base>> wpl_text_chunk_deque_type;

class wpl_text_chunk_it {
	private:
	wpl_text_chunk_deque_type &container;
	wpl_text_chunk_deque_type::iterator it;
	int pos;

	public:
	wpl_text_chunk_it (wpl_text_chunk_deque_type &container) :
		container(container),
		pos(0)
	{
		it = container.begin();
	}

	int get_pos() const {
		return pos;
	}

	wpl_text_chunk_deque_type::iterator &operator++() {
		if (++it == container.end()) {
			throw wpl_text_chunk_end_reached();
		}
		pos++;
		return it;
	}

	wpl_text_chunk_deque_type::iterator &operator++(int) {
		return operator++();
	}

	wpl_text_chunks::base *operator->() {
		return (*it).get();
	}

	bool inc() {
		pos++;
		return (++it != container.end());
	}

	operator bool() const {
		return (it != container.end());
	}
};

class wpl_text : public wpl_runable, public wpl_parseable {
	protected:

	private:
	wpl_text_chunks::base *push_chunk(const char *start, const char *end);

	protected:
	deque<unique_ptr<wpl_text_chunks::base>> chunks;

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

	private:
	void parse_text(wpl_namespace *parent_namespace, wpl_text *text);
	void parse_expression(wpl_namespace *parent_namespace, wpl_expression *exp);

	public:
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
