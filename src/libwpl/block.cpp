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

#include "block.h"
#include "block_state.h"

#include "modifier.h"
#include "blocks.h"

#include "block_if.h"
#include "block_while.h"
#include "block_foreach.h"
#include "block_for.h"
#include "text.h"
#include "parser.h"

#include "global.h"

#include <cstring>

using namespace std;

extern const wpl_global_block *global_block;
extern const wpl_global_block *global_text;

wpl_block::wpl_block() : wpl_block_parser() {}

wpl_state *wpl_block::new_state(wpl_state *state, wpl_namespace_session *nss, wpl_io *io) {
	return new wpl_block_state (state, nss, io, this);
}

void wpl_block::append_child (unique_ptr<wpl_runable> element) {
	child_elements.push_back(std::move(element));
}

void wpl_block::append_child_position() {
	child_positions.emplace_back(get_position());
}

int wpl_block::run_children (wpl_block_state *block_state, wpl_value *final_result) {
	int ret = WPL_OP_NO_RETURN;

	int i = 0;
	for (unique_ptr<wpl_runable> &r : child_elements) {
		try {
			ret = block_state->run_child(r.get(), i, final_result);
		}
		catch (const runtime_error &e) {
			throw wpl_element_exception(e.what(), child_positions[i]);
		}
		if (ret & (WPL_OP_RETURN|WPL_OP_BREAK)) {
			break;
		}
		i++;
	}

	// Notify destructors
	block_state->notify_destructors(block_state);

	return ret;
}

int wpl_block::run(wpl_state *state, wpl_value *final_result) {
	wpl_block_state *block_state = (wpl_block_state*) state;
	block_state->reset();

#ifdef WPL_DEBUG_BLOCKS
	DBG("B: (" << this << "): Run" << endl);
#endif

	return run_children(block_state, final_result);
}

/**
 * @brief Parse a block, ending with }
 *
 * @param Flags to limit which elements we allow depening on current context
 */
void wpl_block::parse_value(wpl_namespace *ns) {
#ifdef WPL_DEBUG_BLOCKS
	DBG("B: (" << this << "): Parse" << endl);
#endif
	set_parent_namespace(ns);
	wpl_matcher_position start_pos = get_position();

	ignore_blockstart();

	char buf[WPL_VARNAME_SIZE+1];
	// Descend into block and find child blocks and elements
	while (!at_end()) {
		const char *prev_pos = get_string_pointer();

		ignore_whitespace();

		if (at_end()) { break; }

		int len = ignore_letter('#');
		len += ignore_string_match(WORD, 0);
		revert_string(len);

		if (len) {
			check_varname_length(len);
			get_string(buf, len);

			const wpl_parse_and_run *block;
			if (!(block = global_block->find_parse_and_run(buf))) {
				block = find_parse_and_run(buf);
			}

			if (block) {
				append_child_position();

				wpl_parse_and_run *new_block = block->new_instance();
				append_child(unique_ptr<wpl_runable>(new_block));

				// Inform the child that we have not parsed any {
				new_block->set_expect_blockstart();
				parse_parse_and_run(new_block);
			}
			else {
				wpl_parseable_identifier *parseable;
				if (!(parseable = global_block->find_parseable(buf))) {
					parseable = find_parseable(buf);
				}

				if (parseable) {
					wpl_parser::parse_parseable_identifier(this, this, parseable);

					ignore_whitespace();
					ignore_letter(';');
				}
				else {
					revert_string(len);
					append_child_position();
					append_child(parse_expression());
				}
			}
		}
		else if (ignore_string ("/*")) {
			wpl_parser::parse_comment(this);
		}
		else if (search_letter ('{')) {
			append_child_position();
			append_child(parse_block());
		}
		else if (ignore_letter ('}')) {
			return;
		}
/*		else if (ignore_letter ('#')) {
			append_child_position();
			append_child(parse_pragma());
		}*/
		else if (search (EXPRESSION, WHITESPACE, false)) {
			append_child_position();
			append_child(parse_expression());
		}
		else {
			ostringstream msg;
			msg << "Syntax error in block near '" << get_string_pointer()[0] << "'";
			THROW_ELEMENT_EXCEPTION(msg.str());
		}
	}
	load_position(start_pos);
	revert_string(1);
	THROW_ELEMENT_EXCEPTION("Missing '}' after block, block began here:");
}

/* From modifier.cpp */
void wpl_modifier_add_all_to_namespace(wpl_namespace *ns);

/* From pragma.cpp */
void wpl_pragma_add_all_to_namespace(wpl_namespace *ns);

#define ADD_TO_NS(classname) \
	{									\
		wpl_parse_and_run *block = new classname(new wpl_block());	\
		ns->add_managed_pointer(block);					\
		ns->register_parse_and_run(block);				\
	}									\

void wpl_block_add_parse_and_run_to_ns(wpl_namespace *ns) {
	ADD_TO_NS(wpl_block_if)
	ADD_TO_NS(wpl_block_while)
	ADD_TO_NS(wpl_block_for)
	ADD_TO_NS(wpl_block_foreach)

	wpl_modifier_add_all_to_namespace(ns);
	wpl_pragma_add_all_to_namespace(ns);

	{
		wpl_parse_and_run *text = new wpl_text(global_text);
		ns->add_managed_pointer(text);
		ns->register_parse_and_run(text);
	}
}
