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

#include "text.h"
#include "expression.h"
#include "operator.h"
#include "operator_types.h"
#include "exception.h"
#include "value_output_trigger.h"
#include "value_bool.h"
#include "value_string.h"
#include "value_unsafe_pointer.h"
#include "output_json.h"

#include <cstdio>
#include <set>
#include <cstring>

wpl_text::chunk::chunk (const char *start, const char *end) {
	this->start = start;
	this->end = end;
	this->expression = NULL;
}

wpl_text::chunk::chunk (wpl_expression *new_expression) :
	expression(new_expression)
{
	this->start = NULL;
	this->end = NULL;
}

wpl_text::chunk::chunk (wpl_text *text, wpl_expression *new_expression) :
	expression(new_expression),
	loop_text(text)
{
	this->start = NULL;
	this->end = NULL;
}

int wpl_text::chunk::run (
		wpl_text_state *state,
		int index,
		wpl_value *final_result,
		wpl_io &io
) {
	static wpl_value_output_trigger output_trigger(io);
	if (expression.get() == nullptr) {
		io.write_immortal (start, end-start);
		return WPL_OP_NO_RETURN;
	}

	if (loop_text.get() != nullptr) {
		int ret;
		wpl_value_bool result;
		while (state->run_expression (expression.get(), index, &result) && result.get()) {
			ret = state->run_text (
				loop_text.get(),
				index,
				final_result,
				io
			);
		}
		return ret;
	}

	return state->run_expression(expression.get(), index, &output_trigger);
}

wpl_text::chunk *wpl_text::push_chunk(const char *start, const char *end) {
	chunks.emplace_back(start, end);
	return &chunks.back();
}

int wpl_text::run(wpl_state *state, wpl_value *final_result, wpl_io &io) {
	wpl_text_state *text_state = (wpl_text_state*) state;
	int index = 0;
	for (chunk &my_chunk : chunks) {
		my_chunk.run(text_state, index, final_result, io);
		index++;
	}
	return WPL_OP_NO_RETURN;
}

int wpl_text::run(wpl_state *state, wpl_value *final_result) {
	return run(state, final_result, state->get_io());
}

int wpl_text::output_json (
		wpl_state *state,
		const set<wpl_value*> &vars,
		wpl_value *final_result
) {
	wpl_text_state *text_state = (wpl_text_state*) state;

	wpl_io &io = state->get_io();

	const char id_string[] = "id=\"";
	const int id_string_len = sizeof(id_string)-1;

	wpl_value_unsafe_pointer unsafe_pointer;
	wpl_value_string tmp_string;
	wpl_value_bool tmp_bool;

	int index = -1;
	bool next_is_id = false;
	int id_exp_index = 0;
	wpl_expression *id_exp = NULL;

	for (chunk &my_chunk : chunks) {
		index++;

		wpl_expression *current_exp = my_chunk.get_expression();
		wpl_text *current_text = my_chunk.get_loop_text();

		if (next_is_id) {
			if (current_exp) {
				id_exp = current_exp;
			}
			next_is_id = false;
			id_exp_index = index;

			continue;
		}

		if (id_exp && current_exp) {
			int ret;
			text_state->run_expression(current_exp, index, &unsafe_pointer);

			wpl_value *value = unsafe_pointer.dereference();

			if (value && vars.find(unsafe_pointer.dereference()) != vars.end()) {
				io << "\"";
				text_state->run_expression (id_exp, id_exp_index, &tmp_string);
				tmp_string.output_json(io);
				io << "\": \"";

				if (current_text) {
					wpl_io_buffer buf;
					while (text_state->run_expression (
								current_exp, index, &tmp_bool
						) && tmp_bool.get()
					)
					{
						ret = text_state->run_text(current_text, index,
								final_result, buf);
					}

					wpl_output_json json_output;
					json_output.output_json(io, buf.c_str(), buf.size());
				}
				else {
					text_state->run_expression (current_exp, index, &tmp_string);
					tmp_string.output_json(io);
				}

				io << "\",\n";

				id_exp = NULL;
				continue;
			}

			id_exp = NULL;
		}

		if (current_text) {
			while (text_state->run_expression (current_exp, index, &tmp_bool) && tmp_bool.get()) {
				int ret = text_state->run_text_output_json (
						current_text,
						index,
						final_result,
						vars
				);
			}
			continue;
		}

		if (!(my_chunk.get_start())) {
			continue;
		}

		const char *start = my_chunk.get_start();
		const char *end = my_chunk.get_end();

		if ((end - start) < id_string_len) {
			continue;
		}

		if (strncmp (end - id_string_len, id_string, id_string_len) != 0) {
			continue;
		}

		next_is_id = true;
	}

	return WPL_OP_OK;
}

int wpl_text::output_as_json_var(wpl_state *state, wpl_value *final_result) {
	wpl_value_string name_string(get_name());

	wpl_io &io = state->get_io();

	io << "\"";
	name_string.output_json(io);
	io << "\": \"";

	wpl_io_buffer buf;
	wpl_output_json json_io;

	run(state, final_result, buf);

	json_io.output_json(io, buf.c_str(), buf.size());

	io << "\",\n";
	return WPL_OP_OK;
}

void wpl_text::parse_value(wpl_namespace *parent_namespace) {
	ignore_string_match(NEWLINE, NON_NEWLINE_WS);

	const char *start = get_string_pointer();
	const char *end;
	int par_level = 1;
	while (par_level > 0 && !at_end()) {
		end = get_string_pointer();
		if (ignore_letter('{')) {
			if (ignore_string("@LOOP")) {
				push_chunk (start, end);

				wpl_text *text =
					new wpl_text();
				wpl_expression *exp =
					new wpl_expression_par_enclosed();

				chunks.emplace_back(text, exp);

				exp->load_position(get_static_position());
				exp->parse_value(parent_namespace);
				load_position(exp->get_static_position());

				ignore_string_match(NEWLINE, NON_NEWLINE_WS);

				text->load_position(get_static_position());
				text->parse_value(parent_namespace);
				load_position(text->get_static_position());

				start = get_string_pointer();
			}
			else if (ignore_string("@")) {
				push_chunk (start, end);

				wpl_expression *exp =
					new wpl_expression_loose_end();
				chunks.emplace_back(exp);

				exp->load_position(get_static_position());
				exp->parse_value(parent_namespace);
				load_position(exp->get_static_position());

				ignore_string_match(WHITESPACE, 0);
				if (!ignore_letter('}')) {
					THROW_ELEMENT_EXCEPTION("Expected '}' after expression-in-TEXT");
				}

				start = get_string_pointer();
			}
			else {
				par_level++;
			}
		}
		else if (ignore_letter('}')) {
			par_level--;
		}
		else {
			if (!ignore_string_match(NON_CURLY|UTF8, 0)) {
				cerr << "Unknown character '" << hex << get_letter(ALL) << "'\n";
				THROW_ELEMENT_EXCEPTION("Syntax error in text-string");
			}
		}
	}

	if (par_level != 0) {
		THROW_ELEMENT_EXCEPTION("Excepted '}' after TEXT-block");
	}

	end = get_string_pointer() - 2;

	while (M_NON_NEWLINE_WHITESPACE (*end)) {
		end--;
	}
	end++;

	if (end > start) {
		push_chunk (start, end);
	}
}
