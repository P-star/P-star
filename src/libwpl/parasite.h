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

/**
 * @brief Objects allocated in other libraries, like MySQL, must be deleted
 in those libraries. This is why we use the suicide() function instead of
 just delete here.
 */
class wpl_parasite_deleter {
	public:
	template<class T> void operator() (T *p) {
		p->suicide();
	}
};

/**
 * @brief Extend this class to create a new parasite type. The parasite is notified with notify() being cold when the value it lives in changes.
 *
 * @tparam T Type of the value which supports being parasite host
 */
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

/**
 * @brief A value can extend this class to become a parasite host. The host must call notify_parasites() every time it changes. Functionality like MySQL statements register parasites on values which are part of SQL queries.
 *
 * @tparam T The value type
 */
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
