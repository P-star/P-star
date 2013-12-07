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

#include <iostream>

#if \
	defined(WPL_DEBUG_EXPRESSIONS) || 	\
	defined(WPL_DEBUG_FUNCTIONS) || 	\
	defined(WPL_DEBUG_PARSING) || 		\
	defined(WPL_DEBUG_DESTRUCTION) || 	\
	defined(WPL_DEBUG_NAMESPACE)

  #define WPL_DEBUG 1
  #define DBG(msg) \
	cerr << msg

#else

  #define DBG(msg)

#endif


