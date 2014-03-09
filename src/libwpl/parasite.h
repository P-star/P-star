/*

-------------------------------------------------------------

Copyright (c) MMXIV Atle Solbakken
atle@goliathdns.no
Copyright (c) MMXIV Sebastian Baginski
sebthestampede@gmail.com


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

#include <memory>
#include <list>

class wpl_parasite_deleter {
	public:
	template<class T> void operator() (T *p) {
		p->suicide();
	}
};

template<class T> class wpl_parasite {
	protected:
	T *host;
	const void *identifier;

	public:
	wpl_parasite (T *host, const void *identifier) : host(host), identifier(identifier) {}
	virtual ~wpl_parasite() {}
	virtual void notify() = 0;
	virtual void suicide() = 0;
	bool check_identifier(const void *identifier) const {
		return (this->identifier == identifier);
	}
};

template<class T> class wpl_parasite_host {
	list<unique_ptr<wpl_parasite<T>,wpl_parasite_deleter>> parasites;

	public:
	wpl_parasite_host(): parasites() {}
	wpl_parasite_host(const wpl_parasite_host<T> &copy): parasites() {}

	void register_parasite(wpl_parasite<T> *parasite) {
		parasites.emplace_back(parasite);
	}

	void notify_parasites() {
		// without the 'const &' it attempts to use deleted unique_ptr copy constructor
		for (const auto &parasite : parasites) {
			parasite->notify();
		}
	}

	template<class U> U *find_parasite(const void *identifier) {
		for (const auto &parasite : parasites) {
			if (parasite->check_identifier(identifier)) {
				return (U*) parasite.get();
			}
		}
		return nullptr;
	}
};
