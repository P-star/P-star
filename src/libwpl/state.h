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

#include "io.h"

#include <sstream>

using namespace std;

class wpl_namespace_session;
class wpl_io;

class wpl_state {
	private:
	wpl_state *parent;
	wpl_namespace_session *nss;
	wpl_io *io;

	int flags;

	public:
	static const int DO_JSON_OUTPUT = 1;

	wpl_state(wpl_state *parent, wpl_namespace_session *nss, wpl_io *io) :
		parent(parent),
		nss(nss),
		io(io),
		flags(0)
	{
		if (parent != NULL) {
			this->flags = parent->flags;
		}
	}

	wpl_state(wpl_io *io) :
		parent(NULL),
		nss(NULL),
		io(io),
		flags(0)
	{}
	wpl_state (const wpl_state &copy) {
		this->nss = copy.nss;
		this->io = copy.io;
	}

	wpl_io &get_io() {
		return *io;
	}
	void set_io(wpl_io &io) {
		this->io = &io;
	}

	wpl_namespace_session *get_nss() {
		return nss;
	}

	template<typename T> T *find_parent() {
		if (parent == NULL) {
			return NULL;
		}
		if (T *ret = dynamic_cast<T*>(parent)) {
			return ret;
		}
		return parent->find_parent<T>();
	}

	void unset_json_output() {
		flags &= (~DO_JSON_OUTPUT);
	}

	void set_json_output() {
		flags |= DO_JSON_OUTPUT;
	}

	bool get_json_output() {
		return (flags & DO_JSON_OUTPUT);
	}

	virtual void reset() {};
	virtual ~wpl_state() {}
};

