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

#include "type_file.h"
#include "value_line.h"
#include "operator_types.h"
#include "value_string.h"
#include "value_struct.h"
#include "value_int.h"

#include <sstream>

void wpl_value_line::check_modify() {
	if (!file || !file->is_open()) {
		throw runtime_error("Cannot set value of LINE when associated FILE object is not open");
	}
	if (!file->is_writeable()) {
		throw runtime_error("Cannot modify LINE object associated with read-only FILE");
	}
	if (!chunk) {
		throw runtime_error("Cannot set value of LINE as it does not yet point to any data. Try line++ first?");
	}
}

void wpl_value_line::set_weak(wpl_value *value) {
	{
		wpl_value_line *value_line = dynamic_cast<wpl_value_line*>(value);
		if (value_line) {
			file = value_line->get_file_shared_ptr();
			chunk = value_line->get_chunk_shared_ptr();
			return;
		}
	}
	{
		wpl_value_struct *value_struct = dynamic_cast<wpl_value_struct*>(value);
		if (!value_struct)
			goto notfile;

		wpl_variable *variable_this = value_struct->get_this();
		if (!variable_this)
			goto notfile;

		wpl_value_file *value_file = dynamic_cast<wpl_value_file*>(variable_this->get_value());
		if (!value_file)
			goto notfile;

		set (value_file->get_file_shared_ptr());
		return;
	}

notfile:
	check_modify();
	chunk->set_data(value->toString());
}

string wpl_value_line::toString() const {
	if (!chunk) {
		throw runtime_error("Cannot get string value of LINE as it does not yet point to any data. Try line++ first?");
	}
	return chunk->get_data();
}

int wpl_value_line::do_operator (
	wpl_expression_state *exp_state,
	wpl_value *final_result,
	const wpl_operator_struct *op,
	wpl_value *lhs,
	wpl_value *rhs
) {
	if (op == &OP_ASSIGN) {
		lhs->set_weak(rhs);
		wpl_value_bool res(1);
		return res.do_operator_recursive(exp_state, final_result);
	}

	if (!file) {
		throw runtime_error("Only assign operator = is valid for LINE objects"
			       " not yet associated with any FILE. Try line=file first?");
	}

	if (op == &OP_ASSIGN_CONCAT) {
		check_modify();
		chunk->append_data (rhs->toString());

		wpl_value_string result (chunk->get_data());
		return result.do_operator_recursive(exp_state, final_result);
	}

	if (op == &OP_INC_SUFFIX || op == &OP_INC_PREFIX) {
		wpl_value_int res(0);

		if (!chunk) {
			chunk = file->new_chunk_begin();
		}
		else {
			chunk = chunk->get_next();
		}

		if (file->check_pos(chunk->get_pos())) {
			file->read_line(chunk.get());
			res.set(chunk->get_size());
		}

		return res.do_operator_recursive(exp_state, final_result);
	}

	if (op == &OP_DEC_SUFFIX || op == &OP_DEC_PREFIX) {
		wpl_value_int res(0);

		if (!chunk) {
			chunk = file->new_chunk_end();
		}
		else {
			chunk.reset(new wpl_file_chunk(*chunk));
		}

		file->read_previous_line(chunk.get());
		res.set(chunk->get_size());

		return res.do_operator_recursive(exp_state, final_result);
	}

	if (!chunk) {
		throw runtime_error("Cannot operate on LINE object because it does"
			       " not yet point to any data. Try line++ first?");
	}

	if (op == &OP_CONCAT) {
		wpl_value_string res(lhs->toString() + rhs->toString());
		return res.do_operator_recursive(exp_state, final_result);
	}

	return WPL_OP_UNKNOWN;
}
