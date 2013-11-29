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

#include "typenames.h"
#include "type_precedence.h"
#include "identifier.h"
#include "namespace.h"
#include "parseable.h"
#include "debug.h"

#include <cmath>
// #include <cstdint> issues compiler error
#include <stdint.h>
#include <memory>

class wpl_type_complete;
class wpl_type_bool;
class wpl_type_string;

class wpl_value;

class wpl_type : public wpl_parseable {
	private:

	public:
	wpl_type(const char *name) : wpl_parseable(name) {}
	virtual ~wpl_type() {
#ifdef WPL_DEBUG_DESTRUCTION
		DBG("T (" << (wpl_identifier*)this << "): Destructing type\n");
#endif
	}

	virtual int get_precedence() const = 0;
	virtual void suicide() = 0;

	bool isType() const {
		return true;
	}

	virtual bool isIncomplete() const {
		return false;
	}
	virtual bool isComplete() const {
		return false;
	}
	virtual bool isTemplate() const {
		return false;
	}
};

class wpl_type_begin_declaration {
	protected:
	wpl_type_complete *type;
	string name;
	int access_flags;

	wpl_type_begin_declaration (
		wpl_type_complete *_type,
		const char *_name,
		int _access_flags
	) :
		type(_type),
		name(_name),
		access_flags(_access_flags)
	{}
	public:
	void set_flags(int access_flags) {
		this->access_flags = access_flags;
	}
};

/**
 * @brief When a complete type finds word and a '(' after itself, this is thrown
 */
class wpl_type_begin_function_declaration : public wpl_type_begin_declaration, public wpl_matcher {
	public:
	wpl_type_begin_function_declaration(
		wpl_type_complete *type,
		const char *name,
		const wpl_matcher_position *pos
	) :
		wpl_type_begin_declaration (type, name, WPL_VARIABLE_ACCESS_PRIVATE),
		wpl_matcher(pos)
	{}
	void parse_value (wpl_namespace *parent_namespace);
};

/**
 * @brief When a complete type finds a word after itself, this is thrown
 */
class wpl_type_begin_variable_declaration : public wpl_type_begin_declaration {
	private:
	const wpl_matcher_position position_at_name;

	public:
	wpl_type_begin_variable_declaration(
			wpl_type_complete *type,
			const char *name,
			const wpl_matcher_position &_position_at_name
	) :
		position_at_name(_position_at_name),
		wpl_type_begin_declaration (type, name, WPL_VARIABLE_ACCESS_PRIVATE)
	{}
	void create_variable(wpl_namespace *parent_namespace);
	const wpl_matcher_position *get_position_at_name() {
		return &position_at_name;
	}
};

/**
 * @brief When a complete type finds a '>' after itself, this is thrown
 */
class wpl_type_end_template_declaration {
	private:
	const wpl_type_complete *type;

	public:
	wpl_type_end_template_declaration (const wpl_type_complete *_type) :
		type(_type)
	{}
	const wpl_type_complete *get_type() {
		return type;
	}
};

/**
 * @brief This is thrown when structs are completely parsed
 */
class wpl_type_end_statement {
	private:
	wpl_matcher_position end_pos;
	public:
	wpl_type_end_statement(const wpl_matcher_position *pos) :
		end_pos(*pos)
	{}
	const wpl_matcher_position *get_static_position() {
		return &end_pos;
	}
};

/**
 * @brief Class for simple types like int, string and array. User-defined types are also considered complete (must parse them first for them to work).
 */
class wpl_type_complete : public wpl_type {
	public:
	wpl_type_complete (const char *name) : wpl_type(name) {}
	virtual ~wpl_type_complete() {
#ifdef WPL_DEBUG_DESTRUCTION
		DBG("T (" << (wpl_identifier*)this << "): Destructing complete type\n");
#endif
	}

	virtual wpl_value *new_instance() const = 0;
	virtual void suicide() = 0;

	void parse_value (wpl_namespace *parent_namespace);

	bool isComplete() const {
		return true;
	}
};

/**
 * @brief Type for template types like array<type>
 */

class wpl_type_complete_template;

class wpl_type_template : public wpl_type {
	private:
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

	int get_precedence() const {
		return mother_type->get_precedence();
	}
};

/**
 * @brief Class for user-defined types, created with keywords like struct and class. These types need more parsing than just the name of the type. Incomplete types become complete when they are parsed.
 */
class wpl_type_user_incomplete : public wpl_type_complete, public wpl_namespace {
	private:

	public:
	wpl_type_user_incomplete(const char *name) :
		wpl_type_complete(name)
	{}
	virtual ~wpl_type_user_incomplete() {
#ifdef WPL_DEBUG_DESTRUCTION
		DBG("T (" << (wpl_identifier*)this << "): Destructing incomplete user type\n");
#endif
	}
	virtual void suicide() = 0;

	virtual void parse_value(wpl_namespace *parent_namespace) = 0;
};

/**
 * @brief Class for types like struct and class
 */
class wpl_type_incomplete : public wpl_type {
	public:
	wpl_type_incomplete (const char *name) : wpl_type(name) {}

	virtual wpl_type_user_incomplete *new_instance(const char *name) const = 0;
	virtual ~wpl_type_incomplete() {
#ifdef WPL_DEBUG_DESTRUCTION
		DBG("T (" << (wpl_identifier*)this << "): Destructing incomplete type\n");
#endif
	}
	virtual void suicide() = 0;
	void parse_value(wpl_namespace *parent_namespace) override;

	bool isIncomplete() const {
		return true;
	}
};

#define DECLARE_COMPLETE_TYPE(name)							\
	class wpl_type_##name : public wpl_type_complete {				\
		public:									\
		wpl_type_##name() : wpl_type_complete(wpl_typename_##name) {}		\
		wpl_type_##name(const char *type) : wpl_type_complete(type) {}		\
		int get_precedence() const { return wpl_type_precedence_##name; }	\
		wpl_value *new_instance () const;					\
		wpl_identifier *clone() const { return new wpl_type_##name(*this); }	\
		void suicide() { delete this; }						\
	};										\
	extern wpl_type_##name *wpl_type_global_##name;

DECLARE_COMPLETE_TYPE(void);
DECLARE_COMPLETE_TYPE(bool);
DECLARE_COMPLETE_TYPE(int);
DECLARE_COMPLETE_TYPE(llint);
DECLARE_COMPLETE_TYPE(uint);
DECLARE_COMPLETE_TYPE(lluint);
DECLARE_COMPLETE_TYPE(float);
DECLARE_COMPLETE_TYPE(double);
DECLARE_COMPLETE_TYPE(string);

DECLARE_COMPLETE_TYPE(env);
DECLARE_COMPLETE_TYPE(get);
DECLARE_COMPLETE_TYPE(post);

#ifndef WIN32
DECLARE_COMPLETE_TYPE(stdin);
#endif

void wpl_types_add_all_to_namespace(wpl_namespace *parent_namespace);
