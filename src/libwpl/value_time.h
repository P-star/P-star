/*

-------------------------------------------------------------

Copyright (c) MMXIII-MMXIV Atle Solbakken
atle@goliathdns.no
Copyright (c) MMXIV Sebastian Baginski
sebthestampede@gmail.com

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
#include "wpl_time.h"
#include "parasite.h"

class wpl_operator_struct;

class wpl_value_time : public wpl_value, public wpl_time, public wpl_parasite_host<wpl_value_time> {
	private:
	void try_guess_from_str(const std::string& fmt);
	void set_from_int(const int value);

	protected:
	public:
	PRIMITIVE_TYPEINFO(time)
	wpl_value_time(int dummy) : wpl_time(), wpl_value(), wpl_parasite_host<wpl_value_time>() {}
	wpl_value_time *clone() const { return new wpl_value_time(*this); };
	wpl_value_time *clone_empty() const { return new wpl_value_time(0); };

	void set_weak(wpl_value *value) override;
	string toString() const override;
	char* get_mysql_time_ptr();

	void output_json(wpl_io &io) override;
	void output(wpl_io &io) override;

	bool toBool() override {
		return get_is_set();
	}

	int toInt() override {
		return get_time();
	}

	int do_operator (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
	);
};
