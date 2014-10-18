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
#include "parse_and_run.h"
#include "expression.h"
#include "text_state.h"
#include "namespace.h"
#include "io.h"

#include <set>
#include <deque>
#include <memory>

class wpl_value;
class wpl_namespace_session;

class wpl_text_chunk_end_reached {};
class wpl_text_chunk_it;

namespace wpl_text_chunks {
	class base {
		private:

		public:
		virtual ~base() {}
		virtual int run (
			wpl_text_state *state,
			int index,
			wpl_value *final_result,
			wpl_io &io
		) = 0;
		virtual int output_json (
			wpl_text_state *state,
			wpl_value *final_result
		) = 0;
		virtual bool isText() {
			return false;
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

		int run(wpl_text_state *state, int index, wpl_value *final_result, wpl_io &io) override;
		virtual int output_json (
			wpl_text_state *state,
			wpl_value *final_result
		);
		virtual bool isText() {
			return true;
		}
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
			wpl_value *final_result
		);
	};

	class runable : public base {
		private:
		unique_ptr<wpl_runable> block;

		public:
		runable (wpl_runable *block) :
			block(block)
		{}
		virtual ~runable() {}

		int run (wpl_text_state *state, int index, wpl_value *final_result, wpl_io &io) override;
		int output_json (
			wpl_text_state *state,
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
		virtual int output_json (
			wpl_text_state *state,
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

class wpl_text : public wpl_parse_and_run {
	protected:

	private:
	wpl_text_chunks::base *push_chunk(const char *start, const char *end);

	const wpl_namespace *global_types;

	protected:
	deque<unique_ptr<wpl_text_chunks::base>> chunks;

	public:
	wpl_text (const wpl_namespace *global_types) :
		wpl_parse_and_run("TEXT"),
		global_types(global_types)
	{};
	wpl_text (const char *name, const wpl_namespace *global_types) :
		wpl_parse_and_run(name),
		global_types(global_types)
	{};
	wpl_text(const wpl_text &copy) :
		wpl_parse_and_run(copy),
		global_types(copy.global_types)
	{}

	virtual void suicide() override {
		delete this;
	}
	virtual ~wpl_text() {}

	wpl_text *new_instance() const override {
		return new wpl_text(*this);
	}

	wpl_state *new_state(wpl_state *parent, wpl_namespace_session *nss, wpl_io *io) override {
		return new wpl_text_state(parent, nss, io, chunks.size());
	}

	private:
	void parse_text(wpl_namespace *parent_namespace, wpl_text *text);
	void parse_expression(wpl_namespace *parent_namespace, wpl_expression *exp);

	public:
	virtual void parse_value(wpl_namespace *parent_namespace) override;
	virtual int run(wpl_state *state, wpl_value *final_result) override;

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
	wpl_state *new_state(wpl_state *parent, wpl_namespace_session *nss, wpl_io *io) {
		return my_text->new_state(parent, nss, io);
	}
	int run (wpl_state *state, wpl_value *final_result) {
		return my_text->output_as_json_var(state, final_result);
	}
};
