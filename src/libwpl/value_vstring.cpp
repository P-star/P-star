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

#include "value_vstring.h"

wpl_value_vstring::vstring::vstring(int length) {
	this->length = length;
	data = new char[length+1];
	data[0] = '\0';
}

wpl_value_vstring::vstring::vstring (const vstring &copy) :
	data(new char[copy.length+1]),
	length(copy.length)
{
	memcpy(data, copy.data, length);
	data[length] = '\0';
}

void wpl_value_vstring::vstring::resize(int new_length) {
	if (new_length < length) {
		data[new_length] = '\0';
		return;
	}

	length = new_length;

	delete[] data;
	data = new char[length+1];
	data[length] = '\0';
}
