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
#include <memory>
#include <map>

class wpl_value_line;

class wpl_file_chunk {
	private:
	int pos;
	int orig_size;
	string data;

	public:
	wpl_file_chunk() :
		pos(0),
		orig_size(0) {}

	wpl_file_chunk(int pos) :
		pos(pos),
		orig_size(0) {}

	void merge (const wpl_file_chunk &chunk) {
		data += chunk.get_data();
	}
	shared_ptr<wpl_file_chunk> get_next() const {
		return shared_ptr<wpl_file_chunk>(new wpl_file_chunk(pos+orig_size));
	}
	int get_pos() const {
		return pos;
	}
	int get_size() const {
		return data.size();
	}
	int get_orig_size() const {
		return orig_size;
	}
	string &get_data() {
		return data;
	}
	const string &get_data() const {
		return data;
	}
	void update_orig_size() {
		orig_size = data.size();
	}
	void set_data(const string &data) {
		this->data = data;
	}
	void append_data(const string &data) {
		this->data += data;
	}
	void set_pos(int pos) {
		this->pos = pos;
		this->data.clear();
	}
};

class wpl_file {
	private:

	fstream file;
	FILE *extra_fd;

	int size;
	int new_size;
	std::ios_base::openmode mode;

	typedef map<int,shared_ptr<wpl_file_chunk>> updates_type;
	updates_type updates;

	ostringstream error;

	public:
	wpl_file() {
		extra_fd = NULL;
		size = 0;
		new_size = 0;
	}
	~wpl_file();

	void reset_error() {
		error.str("");
		error.clear();
	}

	string get_error() {
		return error.str();
	}

	shared_ptr<wpl_file_chunk> new_chunk_begin() {
		return shared_ptr<wpl_file_chunk>(new wpl_file_chunk());
	}

	shared_ptr<wpl_file_chunk> new_chunk_end() {
		return shared_ptr<wpl_file_chunk>(new wpl_file_chunk(size));
	}

	bool check_error();
	bool open (const char *filename, ios_base::openmode mode);
	bool close();
	bool update(wpl_value_line *line);
	bool flush();

	bool is_writeable() {
		return (mode & ios_base::out);
	}
	bool is_open() {
		return file.is_open();
	}
	bool check_pos(int pos) {
		return (pos < size);
	}

	void read_line (wpl_file_chunk *chunk);
	void read_previous_line (wpl_file_chunk *chunk);
};
