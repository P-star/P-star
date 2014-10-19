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

#include "value_holder.h"
#include "exception.h"
#include "wpl_file.h"
#include "type_file.h"

#include <memory>

class wpl_operator_struct;

class wpl_value_line : public wpl_value {
	private:
	void check_modify();

	protected:
	shared_ptr<wpl_file> file;
	shared_ptr<wpl_file_chunk> chunk;

	public:
	PRIMITIVE_TYPEINFO(line)
	wpl_value_line () : wpl_value() {}
	wpl_value_line (int dummy) {}
	wpl_value_line *clone() const { return new wpl_value_line(*this); };
	wpl_value_line *clone_empty() const { return new wpl_value_line(0); };

	void set_weak(wpl_value *value) override;
	bool set_strong(wpl_value *value) override {
		set_weak(value);
		return true;
	}
	string toString() const override;
	bool toBool() override {
		return (chunk) && chunk->get_size();
	}
	int toInt() override {
		return (chunk ? chunk->get_size() : 0);
	}

	int do_operator (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
	);

	void set(const shared_ptr<wpl_file> &file, const shared_ptr<wpl_file_chunk> &chunk) {
		this->file = file;
		this->chunk = chunk;
	}

	void set(const shared_ptr<wpl_file> &file) {
		this->file = file;
		chunk.reset();
	}

	shared_ptr<wpl_file_chunk> &get_chunk_shared_ptr() {
		return chunk;
	}

	shared_ptr<wpl_file> &get_file_shared_ptr() {
		return file;
	}

	bool is_file (const wpl_file *file) const {
		return (file == this->file.get());
	}
};
