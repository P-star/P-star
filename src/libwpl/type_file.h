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

#include "wpl_file.h"
#include "value.h"
#include "struct.h"

#include <memory>

class wpl_type_file : public wpl_struct {
	public:
	wpl_type_file();
	void suicide() override {
		delete this;
	}
	int get_precedence() const {
		return wpl_type_precedence_file;
	}
};

extern wpl_type_file constant_type_file;

class wpl_value_file : public wpl_value {
	private:
	shared_ptr<wpl_file> file;

	public:
	const char *get_type_name() const override { return wpl_typename_file; };
	int get_precedence() const override { return wpl_type_precedence_file; };

	wpl_value_file *clone() const override {
		return new wpl_value_file(*this);
	};

	wpl_value_file *clone_empty() const override {
		return new wpl_value_file();
	};

	shared_ptr<wpl_file> &get_file_shared_ptr() {
		if (!file) {
			file.reset(new wpl_file());
		}
		return file;
	}

	wpl_file *get_file() {
		if (!file) {
			file.reset(new wpl_file());
		}
		return file.get();
	}

	bool set_strong (wpl_value *value) override {
		wpl_value_file *src;
		if (!(src = dynamic_cast<wpl_value_file*>(value))) {
			return false;
		}
		file = src->get_file_shared_ptr();
		return true;
	}
};


