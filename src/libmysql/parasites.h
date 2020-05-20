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

#include <mysql/mysql_time.h>

#include "../libwpl/value_time.h"
#include "../libwpl/parasite.h"

class wpl_mysql_time_parasite : public wpl_parasite<wpl_value_time> {
	MYSQL_TIME sql_time;

	public:
	wpl_mysql_time_parasite (wpl_value_time *host, const void *identifier) :
		wpl_parasite<wpl_value_time>(host, identifier) {}

	void notify() override {
		struct tm time_tmp;
		memset(&sql_time, 0, sizeof(sql_time));
		host->get_time(&time_tmp);
		sql_time.hour = time_tmp.tm_hour;
		sql_time.minute = time_tmp.tm_min;
		sql_time.second = time_tmp.tm_sec;
		sql_time.day = time_tmp.tm_mday;
		sql_time.year = time_tmp.tm_year+1900;
		sql_time.month = time_tmp.tm_mon+1;
		sql_time.time_type = MYSQL_TIMESTAMP_DATETIME;
	}

	char *get() {
		return (char*) &sql_time;
	}

	void suicide() {
		delete this;
	}
};

class wpl_mysql_string_parasite : public wpl_parasite<wpl_value_string> {
	private:
	wpl_namespace_session *nss;
	MYSQL_STMT *stmt;
	wpl_sql *sql;

	int previous_size;
	char *previous_ptr;

	public:
	wpl_mysql_string_parasite (wpl_value_string *host, wpl_namespace_session *nss, MYSQL_STMT *stmt, wpl_sql *sql) :
		wpl_parasite<wpl_value_string>(host, (void*) stmt),
		nss(nss),
		stmt(stmt),
		sql(sql),
		previous_size(-1),
		previous_ptr(nullptr)
	{}

	void notify() override;

	char *get() {
		return (char*) host->toVoid();;
	}

	int get_size() {
		return host->get_size();
	}

	void suicide() {
		delete this;
	}
};
