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

#include "runable.h"
#include "namespace.h"
#include "identifier.h"
#include "debug.h"
#include "function_state.h"

#include <sstream>

using namespace std;

class wpl_value;
class wpl_block;
class wpl_type_complete;

class wpl_function : public wpl_identifier_access_holder, public wpl_runable, public wpl_namespace {
	private:
	char function_name[WPL_VARNAME_SIZE];

	protected:
	const wpl_type_complete *return_type;

	public:
	virtual ~wpl_function() {}
	wpl_function (
		const wpl_type_complete *new_return_type,
		const char *name,
		int access_flags
	) :
		wpl_identifier_access_holder(name, access_flags),
		return_type(new_return_type)
	{}

	const char *get_function_name() const {
		return function_name;
	}

	virtual void suicide() override {
		delete this;
	}

	bool isFunction() const {
		return true;
	}

	wpl_state *new_state(wpl_state *parent, wpl_namespace_session *nss_this, wpl_io *io);
	wpl_state *new_state(
			wpl_state *parent, 
			wpl_namespace_session *nss_call_ctx,
			wpl_namespace_session *nss_this,
			wpl_io *io
	);

	void generate_signature();

	const wpl_type_complete *get_return_type () {
		return return_type;
	}
};

