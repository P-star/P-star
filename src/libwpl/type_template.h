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

#include "type.h"

class wpl_type_complete;
class wpl_type_complete_template;
class wpl_namespace;

/**
 * @brief Type for template types like array<type>
 */
class wpl_type_template : public wpl_type {
	private:
	wpl_type_complete *register_unique_complete_type (wpl_namespace *parent_namespace, const wpl_type_complete *type);

	public:
	wpl_type_template (const char *name) : wpl_type(name) {}
	virtual ~wpl_type_template() {
#ifdef WPL_DEBUG_DESTRUCTION
		DBG("T (" << (wpl_identifier*)this << "): Destructing template type\n");
#endif
	}
	virtual void suicide() = 0;

	bool isTemplate() const {
		return true;
	}

	virtual wpl_type_complete_template *new_instance(const wpl_type_complete *template_type) const = 0;
	void parse_value(wpl_namespace *parent_namespace);
};

/**
 * @brief Type for complete templates
 */
class wpl_type_complete_template : public wpl_type_complete {
	protected:
	const wpl_type_template *mother_type;
	const wpl_type_complete *template_type;

	public:
	wpl_type_complete_template (
			const wpl_type_template *mother_type,
			const wpl_type_complete *template_type
	);
	virtual ~wpl_type_complete_template() {
#ifdef WPL_DEBUG_DESTRUCTION
		DBG("T (" << (wpl_identifier*)this << "): Destructing complete template type\n");
#endif
	}
	virtual void suicide() = 0;

	virtual bool check_type(const wpl_type_complete *) const override;
	bool check_type(const wpl_type_template *mother_type, const wpl_type_complete *template_type) const;

	int get_precedence() const {
		return mother_type->get_precedence();
	}
};

