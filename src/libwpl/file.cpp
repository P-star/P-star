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

#include "wpl_file.h"

#include <errno.h>

wpl_file::~wpl_file() {
	if (!file.is_open()) {
		return;
	}
	file.close();
	if (check_error()) {
		ostringstream msg;
		msg << "Could not close file: " << strerror(errno);
		throw runtime_error(msg.str());
	}
}

bool wpl_file::check_error() {
	if (file.fail() || file.bad()) {
		return true;
	}
	return false;
}

bool wpl_file::open(const char *filename, ios_base::openmode mode) {
	if (file.is_open()) {
		throw runtime_error("FILE cannot be opened twice! Call close() first");
	}

	file.open(filename, mode);

	if (!file) {
		reset_error();
		error << "Could not open file '" << filename << "': " << strerror(errno);
		return false;
	}

	file.seekg(0, file.end);
	size = file.tellg();
	file.seekg(0, file.beg);

	return true;
}

bool wpl_file::close() {
	if (!file.is_open()) {
		throw runtime_error("close() called before successful open()");
	}

	file.close();

	if (check_error()) {
		reset_error();
		error << "Could not close file: " << strerror(errno);
		return false;
	}

	size = 0;

	return true;
}

void wpl_file::read_line (wpl_file_chunk &chunk) {
	if (!file.is_open()) {
		throw runtime_error("Can't read lines from non-open FILE objects");
	}

	int pos = chunk.get_pos();
	string &data = chunk.get_data();

	file.seekg(chunk.get_pos(), file.beg);
	if (check_error()) {
		reset_error();
		error << "Could not seek to position while reading line: " << strerror(errno);
		throw runtime_error (error.str());
	}

	getline(file, data);

	if (check_error()) {
		reset_error();
		error << "Could not get line from file: " << strerror(errno);
		throw runtime_error (error.str());
	}

	if (!file.eof()) {
		data += "\n";
	}
	else if (file.eof() && size > (pos + data.size())) {
		data += "\n";
	}
}
