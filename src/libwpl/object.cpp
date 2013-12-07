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

#include "object.h"

#include "exception.h"

wpl_object::wpl_object (int size) {
	this->size = size;

	// I know... malloc sucks, should use new instead etc...
	pointer = malloc(size);

	if (pointer == NULL) {
		throw runtime_error("wpl_object::wpl_object(): Could not allocate memory");
	}
}
void wpl_object::write (void *bytes, int length) {
	if (length > size) {
		pointer = realloc(pointer, length);
		if (pointer == NULL){
			throw runtime_error("wpl_object::write(): Could not reallocate memory");
		}
		size = length;
	}
	memcpy (pointer, bytes, length);
}
