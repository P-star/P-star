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

#include "../libwpl/value_string.h"
#include "sql.h"
#include <mysql/mysql.h>

#include "parasites.h"
#include "mysql_stmt.h"

void wpl_mysql_string_parasite::notify() {
	// We must bind again if the size of the string changes
	if ((host->get_size() != previous_size) || (host->toVoid() != previous_ptr)) {
		wpl_mysql_bind (nss, stmt, sql);
	}

	previous_size = host->get_size();
	previous_ptr = host->toVoid();
}
