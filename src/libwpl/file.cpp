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
#include "value_line.h"

#include <cstring>
#include <memory>
#include <errno.h>
#include <cstdio>

#ifndef WIN32
#include <unistd.h>
#endif

wpl_file::~wpl_file() {
	if (!file.is_open()) {
		return;
	}

	file.close();

	if (extra_fd) {
		fclose (extra_fd);
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

	this->mode = mode;

	// Open the file
	file.clear();
	file.open(filename, mode);

	if (!file) {
		reset_error();
		error << "Could not open file '" << filename << "': " << strerror(errno);
		return false;
	}

	if (extra_fd) {
		throw runtime_error("extra_fd is already set in wpl_file::open()");
	}

	// Open the file again using the C-library in case we use ftruncate() later
	if (mode & file.out) {
		extra_fd = fopen(filename, "r+");
		if (!extra_fd) {
			reset_error();
			error << "Could not open file '" << filename << "' with fopen(): " << strerror(errno);
		}
	}

	file.seekg(0, file.end);
	size = file.tellg();
	file.seekg(0, file.beg);

	new_size = size;

	return true;
}

bool wpl_file::close() {
	if (!file.is_open()) {
		throw runtime_error("close() called before successful open()");
	}

	if (updates.size() != 0) {
		throw runtime_error("close() called without pending updates being written first");
	}

	file.close();

	if (check_error()) {
		reset_error();
		error << "Could not close file: " << strerror(errno);
		return false;
	}

	if (extra_fd && fclose(extra_fd)) {
		reset_error();
		error << "Could not close file with fclose(): " << strerror(errno);
		throw runtime_error(error.str());
	}

	extra_fd = NULL;
	size = 0;
	new_size = 0;

	return true;
}

bool wpl_file::update(wpl_value_line *line) {
	if (!line->is_file(this)) {
		throw runtime_error("Attempted to update FILE with a LINE object associated with another FILE object");
	}

	shared_ptr<wpl_file_chunk> chunk = line->get_chunk_shared_ptr();

	new_size -= chunk->get_orig_size();
	new_size += chunk->get_size();

	auto it = updates.find(chunk->get_pos());
	if (it != updates.end()) {
		new_size -= (*it).second->get_size();
		new_size += (*it).second->get_orig_size();
		updates.erase(it);
	}

	updates[chunk->get_pos()] = chunk;
}

bool wpl_file::flush() {
	if (!file.is_open()) {
		throw runtime_error("update() called before successful open()");
	}
	if (!(mode & std::ios_base::out)){
		throw runtime_error("update() called on a FILE object opened read-only");
	}
	if (updates.size() == 0) {
		return false;
	}

	// Insert dummy chunk at the end to copy the end of the file
	if (updates.find(size) == updates.end()) {
		updates[size] = shared_ptr<wpl_file_chunk>(new wpl_file_chunk(size));
	}

	unique_ptr<char[]> out_data(new char[new_size+1]);

	int in_pos = 0;
	char *out_pos = out_data.get();

	for (auto it = updates.begin(); it != updates.end(); ++it) {
		int chunk_pos = it->first;
		shared_ptr<wpl_file_chunk> chunk = it->second;

		// Copy unchanged data from the previous position
		if (in_pos != chunk_pos) {
			int diff = chunk_pos - in_pos;
			file.seekg(in_pos, file.beg);
			file.read(out_pos, diff);
			out_pos += diff;
			in_pos += diff;
			if (check_error()) {
				goto fstream_err;
			}
		}

		// Insert new data
		const string &chunk_data = chunk->get_data();
		strncpy(out_pos, chunk_data.c_str(), chunk_data.size());

		out_pos += chunk_data.size();
		in_pos += chunk->get_orig_size();

		// Update the chunk
		chunk->update_orig_size();

		if ((out_pos-out_data.get()) > new_size) {
			throw runtime_error("Buffer overflow while copying file to temporary memory location");
		}
	}

	*out_pos = '\0';

	// Truncate the original file
#ifdef WIN32
	// XXX - Windows-stuff not tested
	{
		int fd = _fileno(extra_fd);
		if (_chsize (fd, new_size)) {
			goto ftruncate_err;
		}
	}
#else
	{
		int fd = fileno(extra_fd);
		if (ftruncate(fd, new_size)) {
			goto ftruncate_err;
		}
	}
#endif

	file.seekg(0, file.beg);
	file.write(out_data.get(), new_size);
	file.flush();

	updates.clear();
	size = new_size;

	return true;

fstream_err:
	reset_error();
	error << "Error while reading file contents in update(): " << strerror(errno);
	throw runtime_error (error.str());

ftruncate_err:
	reset_error();
	error << "Error while truncating file in update(): " << strerror(errno);
	throw runtime_error (error.str());
	
}

void wpl_file::read_line (wpl_file_chunk *chunk) {
	if (!file.is_open()) {
		throw runtime_error("Can't read lines from non-open FILE objects");
	}

	int pos = chunk->get_pos();
	string &data = chunk->get_data();

	file.seekg(pos, file.beg);
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
