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

/*
   TODO
   Change to const int or enum and use lowercase (boring work), postponed
 */

#define WPL_OP_NO_RETURN	0
#define WPL_OP_OK		(1<<0)
#define WPL_OP_UNKNOWN		(1<<1)
#define WPL_OP_RETURN		(1<<2)
#define WPL_OP_BREAK		(1<<3)
#define WPL_OP_BLOCK_RETURN	(1<<4)
#define WPL_OP_DISCARD		(1<<5)
#define WPL_OP_LOGIC_OK		((1<<6)|(WPL_OP_OK))
#define WPL_OP_NAME_RESOLVED	(1<<7)
#define WPL_OP_NAME_UNRESOLVED	(1<<8)
#define WPL_OP_RETURN_REFERENCE	(1<<9)
#define WPL_OP_RETURN_ITERABLE	(1<<10)
#define WPL_OP_RETURN_VARIABLE	(1<<11)
#define WPL_OP_RETURN_FUNCTION	(1<<12)
#define WPL_OP_RETURN_CONSTANT	(1<<13)
#define WPL_OP_ARG_MISMATCH	(1<<14)
#define WPL_OP_BOOL_TRUE	(1<<15)
#define WPL_OP_BOOL_FALSE	(1<<16)
#define WPL_OP_DATA_MODIFIED	(1<<17)
#define WPL_OP_FUNCTION_DID_RUN	(1<<18)
#define WPL_OP_RANGE		(1<<19)
#define WPL_OP_RANGE_COMPLETE	(1<<20)
#define WPL_OP_RANGE_ABORT	(1<<21)
#define WPL_OP_ASSIGN		(1<<22)

