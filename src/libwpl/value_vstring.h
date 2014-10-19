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

#pragma once

#include "value.h"
#include "value_holder.h"
#include "output_json.h"

#include <cstring>
#include <memory>
#include <algorithm>

using namespace std;

class wpl_value_vstring : public wpl_value {
	private:
	class vstring {
		private:
		int length;
		char *data;

		public:
		vstring(int length);
		vstring (const vstring &copy);
		void resize(int new_length);

		~vstring() {
			delete[] data;
		}

		int size() {
			return length;
		}
		char* operator* () {
			return data;
		}
		const char* operator* () const {
			return data;
		}
		char &operator[] (const int i) {
			return data[i];
		}
	};
	vstring my_vstring;

	public:
	PRIMITIVE_TYPEATTR(vstring)

	wpl_value_vstring(const char *new_value, int len) :
		my_vstring(len)
	{
#ifdef WIN32
		strncpy_s(*my_vstring, len, new_value, len);
#else
		strncpy(*my_vstring, new_value, len);
#endif
	}

	wpl_value_vstring(int len) :
		my_vstring(len)
	{
		my_vstring[0] = '\0';
	}

	wpl_value_vstring() :
		my_vstring(16)
	{
		my_vstring[0] = '\0';
	}

	wpl_value_vstring *clone() const {
		return new wpl_value_vstring(*this);
	}

	void resize(int length) override {
		my_vstring.resize(length);
	}

	bool toBool() {
		return (my_vstring[0] != '\0');
	}
	int toInt() {
		return strtol (*my_vstring, NULL, 0);
	}
	int long long toLongLongInt() {
		return strtoll (*my_vstring, NULL, 0);
	}
	unsigned int toUint() {
		return strtoul (*my_vstring, NULL, 0);
	}
	unsigned long long int toLongLongUint() {
		return strtoull (*my_vstring, NULL, 0);
	}
	float toFloat() {
		return strtof (*my_vstring, NULL);
	}
	double toDouble() {
		return strtod (*my_vstring, NULL);
	}
	string toString() const {
		string ret(*my_vstring);
		return ret;
	}
	char *toVoid() {
		return *my_vstring;
	}
	void output(wpl_io &io) override {
		io << *my_vstring;
	}
	void output_json(wpl_io &io) override {
		wpl_output_json worker;
		worker.output_json(io, *my_vstring, my_vstring.size());
	}
};
