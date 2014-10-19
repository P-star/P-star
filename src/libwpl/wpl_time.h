/*

-------------------------------------------------------------

Copyright (c) MMXIII-MMXIV Atle Solbakken
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

#include <ctime>

static const char *format_rfc2822 = "%a, %d %b %Y %T %z";

/**
 * @brief TIME implementation
 */
class wpl_time {
	private:
	time_t timestamp;
	const char *format;
	string format_string;
	bool is_set = false;

	public:
	wpl_time() : timestamp(0), format(format_rfc2822) {}
	wpl_time(const wpl_time &copy) {
		timestamp = copy.timestamp;

		// We have to copy from the pointer to make
		// sure that we keep the memory valid
		format_string = copy.format;
		format = format_string.c_str();
		is_set = copy.is_set;
	}

	bool get_is_set() {
		return is_set;
	}

	void set_time (time_t src) {
		is_set = true;
		timestamp = src;
	}

	time_t get_time() const {
		return timestamp;
	}

	time_t get_time(struct tm *dst) const {
		localtime_r(&timestamp, dst);
        return timestamp;
	}

    void set_format(const std::string& str){
        format_string = str;
        format = format_string.c_str();
    }

	void set_format(const char *format_) {
		format_string = format_;
		format = format_string.c_str();
	}

	const char *get_format() {
		return format;
	}

	void format_time(const char *format, string &dst) const {
		if (format == NULL) {
			format = this->format;
		}

		char buf[512];
		struct tm time_tmp;
		get_time(&time_tmp);

		int res = strftime(buf, 512, format, &time_tmp);
		if (res <= 0) {
			throw runtime_error("Time string exceeds maximum size of 512 bytes");
		}

		dst = buf;
	}

	void set_now() {
		is_set = true;
		time(&timestamp);
	}
};

