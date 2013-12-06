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

#include "exception.h"

#include <sstream>
#include <iostream>
#include <cstring>
#include <cstdio>

using namespace std;

class wpl_io {
	private:
	char buf[256];

	public:
	wpl_io &operator<< (const char *rhs) {
		write(rhs, strlen(rhs));
		return *this;
	}
	wpl_io &operator<< (const string &rhs) {
		write(rhs.c_str(), rhs.size());
		return *this;
	}
	wpl_io &operator<< (const long int &rhs) {
		sprintf(buf, "%ld", rhs);
		write(buf, strlen(buf));
		return *this;
	}
	wpl_io &operator<< (const long long int &rhs) {
		sprintf(buf, "%lld", rhs);
		write(buf, strlen(buf));
		return *this;
	}
	wpl_io &operator<< (const long unsigned int &rhs) {
		sprintf(buf, "%lu", rhs);
		write(buf, strlen(buf));
		return *this;
	}
	wpl_io &operator<< (const long long unsigned int &rhs) {
		sprintf(buf, "%llu", rhs);
		write(buf, strlen(buf));
		return *this;
	}
	wpl_io &operator<< (const float &rhs) {
		sprintf(buf, "%f", rhs);
		write(buf, strlen(buf));
		return *this;
	}
	wpl_io &operator<< (const double &rhs) {
		sprintf(buf, "%f", rhs);
		write(buf, strlen(buf));
		return *this;
	}
	wpl_io &operator<< (const bool &rhs) {
		sprintf(buf, (rhs ? "true" : "false"));
		write(buf, strlen(buf));
		return *this;
	}
	void write (const string &str) {
		write(str.c_str(), str.size());
	}

	virtual void read (char *str, int len) = 0;
	virtual void write (const char *str, int len) = 0;
	virtual const char *getenv (const char *name) = 0;
	virtual void http_header(const char *field, const char *str) = 0;
	virtual void debug (const char *str) = 0;

	virtual void write_immortal (const char *str, int len) {
		write(str, len);
	}
};

class wpl_io_standard : public wpl_io {
	private:
	public:
	void read (char *str, int len) override {
		std::cin.read(str, len);
	}

	void write (const char *str, int len) override {
		std::cout.write(str, len);
	}

	const char *getenv (const char *name) override;
	void http_header(const char *field, const char *str) override;

	void debug (const char *str) override {
		std::cerr << str;
	}
};

class wpl_io_buffer : public wpl_io {
	private:
	string buffer;

	public:
	void read (char *str, int len) override {
		throw runtime_error("read() not supported for wpl_io_buffer");
	}

	void write (const char *str, int len) override {
		buffer.append(str, len);
	}

	const char *getenv (const char *name) override {
		throw runtime_error("getenv() not supported for wpl_io_buffer");
	}

	void http_header(const char *field, const char *str) override {
		throw runtime_error("http_header() not supported for wpl_io_buffer");
	}

	void debug (const char *str) override {
		buffer += string(str);
	}

	void output (wpl_io &target) {
		target.write(buffer);
	}

	const char *c_str() const {
		return buffer.c_str();
	}

	int size() const {
		return buffer.size();
	}
};
