/*

-------------------------------------------------------------

Copyright (c) MMXIV Atle Solbakken
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

#include "template.h"
#include "scene.h"
#include "pragma_names.h"
#include "namespace_session.h"
#include "pragma_state.h"
#include "expression.h"
#include "parse_and_run.h"

#include <cstring>

class wpl_value;
class wpl_namespace;

class wpl_pragma : public wpl_parse_and_run {
	private:
	const char terminator;

	public:
	wpl_pragma (const char *name, const char terminator) :
		wpl_parse_and_run(name),
		terminator(terminator)
	{
		if (terminator != ';' && terminator != '}') {
			throw runtime_error("wrong terminator");
		}
	}
	wpl_pragma (const wpl_pragma &copy) :
		wpl_parse_and_run(copy),
		terminator(copy.terminator)
	{
	}
	virtual ~wpl_pragma() {}
	virtual wpl_pragma *new_instance() const = 0;
	virtual void suicide() override {
		delete this;
	}
	wpl_state *new_state(wpl_state *parent, wpl_namespace_session *nss, wpl_io *io) override {
		return new wpl_pragma_state(parent, nss, io);
	}
	void parse_default_end();
	virtual void parse_value(wpl_namespace *parent_namespace) = 0;
	bool isPragma() const {
		return true;
	}
};

class wpl_pragma_fixed_text : public wpl_pragma {
	private:
	const char *text;

	public:
	wpl_pragma_fixed_text(const char *text, const char *name, const char terminator) :
		wpl_pragma (name, terminator)
	{
		this->text = text;
	}
	wpl_pragma *new_instance() const {
		return new wpl_pragma_fixed_text(*this);
	}
	void parse_value(wpl_namespace *parent_namespace);
	int run(wpl_state *state, wpl_value *final_result);
};

class wpl_pragma_json_begin : public wpl_pragma_fixed_text {
	public:
	wpl_pragma_json_begin (const char terminator) :
		wpl_pragma_fixed_text (
				"Content-type: application/json\r\n\r\n{\n",
				wpl_pragma_name_json_begin,
				terminator
				)
		{
		}
};

class wpl_pragma_json_end : public wpl_pragma_fixed_text {
	public:
	wpl_pragma_json_end (const char terminator) :
		wpl_pragma_fixed_text ("\t\"\": \"\"\n}\n", wpl_pragma_name_json_end, terminator) {
	}
};

class wpl_pragma_template_var : public wpl_pragma {
	protected:
	wpl_template *my_template;
	unique_ptr<wpl_expression> value_expression;

public:
	wpl_pragma_template_var(const char terminator) :
		wpl_pragma (wpl_pragma_name_template_var, terminator)
	{
		my_template = NULL;
	}
	wpl_pragma_template_var(const wpl_pragma_template_var &copy) :
		wpl_pragma (copy)
	{
		my_template = copy.my_template;
	}
	virtual wpl_pragma_template_var *new_instance() const {
		return new wpl_pragma_template_var(*this);
	}
	int run (wpl_state *state, wpl_value *final_result) override;
	void parse_value(wpl_namespace *parent_namespace);
};

class wpl_pragma_template : public wpl_pragma {
	protected:
	wpl_template *my_template;
	unique_ptr<wpl_expression> exp;

	wpl_template *get_template(wpl_pragma_state *pragma_state);

	public:
	wpl_pragma_template(const char *name, const char terminator) :
		wpl_pragma (name, terminator)
	{
		my_template = NULL;
	}
	wpl_pragma_template(const char terminator) :
		wpl_pragma (wpl_pragma_name_template, terminator)
	{
		my_template = NULL;
	}
	wpl_pragma_template (const wpl_pragma_template &copy) :
		wpl_pragma (copy)
	{
		my_template = NULL;
	}
	virtual wpl_pragma_template *new_instance() const {
		return new wpl_pragma_template(*this);
	}
	virtual int run(wpl_state *state, wpl_value *final_result) override;
	void parse_value(wpl_namespace *parent_namespace);
};

class wpl_pragma_template_as_var : public wpl_pragma_template {
	public:
	wpl_pragma_template_as_var(const char terminator) :
		wpl_pragma_template (wpl_pragma_name_template_as_var, terminator)
	{}
	wpl_pragma_template_as_var *new_instance() const {
		return new wpl_pragma_template_as_var(*this);
	}
	int run(wpl_state *state, wpl_value *final_result) override;
};

class wpl_pragma_scene : public wpl_pragma {
	private:
	wpl_scene *my_scene;

	public:
	wpl_pragma_scene(const char terminator) :
		wpl_pragma (wpl_pragma_name_scene, terminator)
	{}
	virtual wpl_pragma_scene *new_instance() const {
		return new wpl_pragma_scene(*this);
	}
	int run(wpl_state *state, wpl_value *final_result) override {
		wpl_pragma_state *pragma_state = (wpl_pragma_state*) state;
		return pragma_state->run_child(my_scene, 0, final_result);
	}
	void parse_value(wpl_namespace *parent_namespace) {
		char value[WPL_VARNAME_SIZE];
		get_word(value);
		if (!(my_scene = parent_namespace->find_scene(value))) {
			revert_string(strlen(value));
			THROW_ELEMENT_EXCEPTION("Could not find scene");
		}
		ignore_string_match(WHITESPACE, 0);
		if (!ignore_letter(';')) {
			THROW_ELEMENT_EXCEPTION("Expected ';' after #SCENE-pragma");
		}
	}
};

class wpl_pragma_text : public wpl_pragma {
	private:
	const char *text_start;
	const char *text_end;

	void set_text (const char *start, const char *end) {
		text_start = start;
		text_end = end;
	}

	protected:
	void add_to_string (string &buf) {
		buf.append(text_start, (text_end-text_start));
	}

	public:
	virtual ~wpl_pragma_text() {}
	wpl_pragma_text(const char *name, const char terminator) :
		wpl_pragma(name, terminator)
	{}

	virtual void parse_value(wpl_namespace *parent_namespace);
};

class wpl_pragma_dump_file : public wpl_pragma {
	private:
	unique_ptr<wpl_expression> value_expression;
	string static_filename;

	public:
	wpl_pragma_dump_file(const char terminator) :
		wpl_pragma(wpl_pragma_name_dump_file, terminator)
	{}
	wpl_pragma_dump_file *new_instance() const {
		return new wpl_pragma_dump_file(*this);
	}
	wpl_pragma_dump_file (const wpl_pragma_dump_file &copy) : wpl_pragma(copy) {}
	int run(wpl_state *state, wpl_value *final_result) override;
	void parse_value(wpl_namespace *parent_namespace) override;
};

class wpl_pragma_text_content_type : public wpl_pragma_text {
	public:
	wpl_pragma_text_content_type(const char terminator) :
		wpl_pragma_text (wpl_pragma_name_content_type, terminator)
	{}
	wpl_pragma_text_content_type *new_instance() const {
		return new wpl_pragma_text_content_type(*this);
	}
	int run(wpl_state *state, wpl_value *final_result) override;
};

void wpl_pragma_add_all_to_namespace (wpl_namespace *my_namespace);

