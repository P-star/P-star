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

#include "exception.h"

#include <fstream>
#include <map>

class wpl_file_chunk{
	private:
	int pos;
	int orig_length;
	string data;

	public:
	wpl_file_chunk() : pos(0), orig_length(0) {}
	wpl_file_chunk(int pos) : pos(pos), orig_length(0) {}

	wpl_file_chunk get_next() const {
		return wpl_file_chunk(pos+data.size());
	}
	int get_pos() const {
		return pos;
	}
	string &get_data() {
		return data;
	}
	void update_orig_length() {
		orig_length = data.size();
	}
	void set_data(const string &data) {
		this->data = data;
	}
};

class wpl_file {
	private:
	fstream file;
	ostringstream error;
	int size;

	map<int,wpl_file_chunk> updates;

	public:
	wpl_file() {}
	~wpl_file();

	void reset_error() {
		error.str("");
		error.clear();
	}

	string get_error() {
		return error.str();
	}

	void queue_update(const wpl_file_chunk &chunk) {
		updates[chunk.get_pos()] = chunk;
	}

	bool check_error();
	bool open (const char *filename, ios_base::openmode mode);
	bool close();

	bool is_open() {
		return file.is_open();
	}

	bool check_pos(int pos) {
		return (pos < size);
	}

	void read_line (wpl_file_chunk &chunk);
};
