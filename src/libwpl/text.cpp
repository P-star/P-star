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

#include "text.h"
#include "expression.h"
#include "operator.h"
#include "operator_types.h"
#include "exception.h"
#include "value_output_trigger.h"
#include "value_bool.h"
#include "value_string.h"
#include "value_unsafe_pointer.h"
#include "value_double_finalizer.h"
#include "output_json.h"

#include <cstdio>
#include <set>
#include <cstring>

/*
   TODO
   - Replace wpl_io_buffer with wpl_io_json_wrapper for better performance

   */

wpl_text_chunks::base *wpl_text::push_chunk(const char *start, const char *end) {
	wpl_text_chunks::base *chunk = new wpl_text_chunks::text(start, end);
	chunks.emplace_back(chunk);
	return chunk;
}

int wpl_text_chunks::text::run (
		wpl_text_state *state,
		int index,
		wpl_value *final_result,
		wpl_io &io
) {
	io.write_immortal(start, end-start);
	return WPL_OP_NO_RETURN;
}

int wpl_text_chunks::textblock::run (
		wpl_text_state *state,
		int index,
		wpl_value *final_result,
		wpl_io &io
) {
	return state->run_text(text.get(), index, final_result, io);
}

int wpl_text_chunks::expression::run (
		wpl_text_state *state,
		int index,
		wpl_value *final_result,
		wpl_io &io
) {

   	wpl_value_output_trigger output_trigger(io);
	wpl_value_double_finalizer finalizer(&output_trigger, final_result);

	return state->run_expression (exp.get(), index, &finalizer);
}

int wpl_text_chunks::expression::run_raw (
		wpl_text_state *state,
		int index,
		wpl_value *final_result
) {
	return state->run_expression (exp.get(), index, final_result);
}

int wpl_text_chunks::loop::run (
		wpl_text_state *state,
		int index,
		wpl_value *final_result,
		wpl_io &io
) {
	wpl_value_bool result;
	result.set_do_finalize();

	int ret = WPL_OP_NO_RETURN;
	while ((state->run_expression (exp.get(), index, &result) & WPL_OP_OK) && result.get()) {
		ret = state->run_text(get_text(), index, final_result, io);
	}
	return ret;
}

int wpl_text_chunks::text::output_json (
		wpl_text_state *state,
		const set<wpl_value*> &vars,
		wpl_text_chunk_it *it,
		wpl_value *final_result
) {
	const char id_string[] = "id=\"";
	const int id_string_len = sizeof(id_string)-1;

	wpl_io &io = state->get_io();
	wpl_io_void io_void;
	wpl_value_string json_name;
	wpl_value_string json_value;
	wpl_value_unsafe_pointer unsafe_pointer;

	json_name.set_do_finalize();
	json_value.set_do_finalize();
	unsafe_pointer.set_do_finalize();

	/*
	   If we end with an HTML id tag, use this the text value of the
	   next chunk as the JSON name.
	   */
	if (strncmp (end - id_string_len, id_string, id_string_len) != 0) {
		return (*(++(*it)))->output_json(state, vars, it, final_result);
	}

	/*
	   If no return from the next block, just proceed with output_json
	   */
	if (!((*(++(*it)))->run(state, it->get_pos(), &json_name, io_void) & WPL_OP_OK)) {
		return (*it)->output_json(state, vars, it, final_result);
	}

	/*
	   The next block which returns WPL_OP_OK is the expression
	   whos pointer is used to match against the list of requested
	   variables to output.

	 */
	while (true) {
	   	// it++ throws wpl_text_chunk_end_reached() when we're done
		(*it)++;

		/*
		   Check if the next chuck is an expression with return value
		   */
		if (!((*it)->run(state, it->get_pos(), &unsafe_pointer, io_void) & WPL_OP_OK)) {
			continue;
		}

		/*
		   Check if the value exists in the list
		   */
		if (vars.find(unsafe_pointer.dereference()) == vars.end()) {
			continue;
		}

		/*
		   All OK, output this variable
		   */
		(*it)->run(state, it->get_pos(), &json_value, io_void);
		io << "\"";
		json_name.output_json(io);
		io << "\": \"";
		json_value.output_json(io);
		io << "\",\n";

		(*it)++;

		return (*it)->output_json(state, vars, it, final_result);
	}

	return WPL_OP_NO_RETURN;
}

int wpl_text_chunks::textblock::output_json (
		wpl_text_state *state,
		const set<wpl_value*> &vars,
		wpl_text_chunk_it *it,
		wpl_value *final_result
) {
	state->run_text_output_json(text.get(), it->get_pos(), vars, final_result);
	return (*(++(*it)))->output_json(state, vars, it, final_result);
}

int wpl_text_chunks::expression::output_json (
		wpl_text_state *state,
		const set<wpl_value*> &vars,
		wpl_text_chunk_it *it,
		wpl_value *final_result
) {
/*	wpl_io_buffer buf;
	wpl_value_output_trigger output_trigger(buf);

	state->run_expression(exp.get(), it->get_pos(), &output_trigger);

	wpl_output_json output_json;
	output_json.output_json(state->get_io(), buf.c_str(), buf.size());
*/
	return (*(++(*it)))->output_json(state, vars, it, final_result);
}

int wpl_text_chunks::loop::output_json (
		wpl_text_state *state,
		const set<wpl_value*> &vars,
		wpl_text_chunk_it *it,
		wpl_value *final_result
) {
	wpl_value_bool result;
	result.set_do_finalize();

	while ((state->run_expression (exp.get(), it->get_pos(), &result) & WPL_OP_OK) && result.get()) {
		 state->run_text_output_json(get_text(), it->get_pos(), vars, final_result);
	}

	return WPL_OP_NO_RETURN;
}

int wpl_text::run(wpl_state *state, wpl_value *final_result) {
	return run(state, final_result, state->get_io());
}

int wpl_text::run(wpl_state *state, wpl_value *final_result, wpl_io &io) {
	wpl_text_state *text_state = (wpl_text_state*) state;
	int index = 0;
	for (auto &my_chunk : chunks) {
		my_chunk->run(text_state, index, final_result, io);
		index++;
	}
	return WPL_OP_NO_RETURN;
}

int wpl_text::output_json (
		wpl_state *state,
		const set<wpl_value*> &vars,
		wpl_value *final_result
) {
	wpl_text_state *text_state = (wpl_text_state*) state;

	wpl_io &io = state->get_io();
	wpl_text_chunk_it it(chunks);

	try {
		it->output_json(text_state, vars, &it, final_result);
	}
	catch (wpl_text_chunk_end_reached &e) {
		/* Iteration complete :-) */
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

				chunks.emplace_back(new wpl_text_chunks::loop(text, exp));

				exp->load_position(get_position());
				exp->parse_value(parent_namespace);
				load_position(exp->get_position());

				ignore_string_match(NEWLINE, NON_NEWLINE_WS);

				text->load_position(get_position());
				text->parse_value(parent_namespace);
				load_position(text->get_position());

				start = get_string_pointer();
			}
			else if (ignore_string("@")) {
				push_chunk (start, end);

				wpl_expression *exp =
					new wpl_expression_loose_end();
				chunks.emplace_back(new wpl_text_chunks::expression(exp));

				exp->load_position(get_position());
				exp->parse_value(parent_namespace);
				load_position(exp->get_position());

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
