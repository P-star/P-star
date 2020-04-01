/*

-------------------------------------------------------------

Copyright (c) MMXIII-MMXIX Atle Solbakken
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

#include "shunting_yard_carrier.h"
#include "operator_return_values.h"
#include "typenames.h"
#include "type_precedence.h"
#include "exception.h"
#include "suicidal.h"
#include "debug.h"
#include "io.h"

#include <list>
#include <memory>

#if defined(_MSC_VER)
#define strtoll _strtoi64
#define strtoull _strtoui64
#define strtof strtod
#endif

const int wpl_value_no_flags = 0;
const int wpl_value_is_constant = 1;
const int wpl_value_is_dynamic = 2;
const int wpl_value_do_finalize = 4;

struct wpl_operator_struct;
class wpl_namespace_session;
class wpl_expression_state;
class wpl_type_complete;
class wpl_value_bool;
class wpl_pointer;
class wpl_value;

class wpl_value_no_strong_set : public runtime_error {
	public:
	wpl_value_no_strong_set() :
		runtime_error("Strong set not possible for type")
	{}
};
class wpl_value_no_weak_set : public runtime_error {
	public:
	wpl_value_no_weak_set() :
		runtime_error("Weak set not possible for type")
	{}
};

/**
 * @brief This class should be allocated on the stack before running runables. Set the result from the runable with set(), and the value will be automatically deleted on destruction of this class if WPL_OP_RETURN_REFERENCE is set.
 */
/*
class wpl_value_return {
	private:
	wpl_value *value;
	bool do_delete;
	bool iterable;

	wpl_value *iterate();

	public:
	wpl_value_return (const wpl_value_return &copy) {
		clear();
	}
	wpl_value_return () {
		clear();
	}
	wpl_value_return (wpl_value *value, int flags) {
		clear();
		set(value, flags);
	}
	~wpl_value_return();
	wpl_value *get() {
		if (iterable) {
			return iterate();
		}
		return value;
	}
	void clear() {
		value = NULL;
		do_delete = false;
		iterable = false;
	}
	void set(wpl_value *value, int flags);
	int run(wpl_value **final_result);
};
*/
/**
 * @brief This is the master value class. All values derive from this type. Values support doing operators and to be converted between one another.
 */
class wpl_value : public wpl_suicidal {
	private:
	vector<wpl_pointer*> pointers;
	void invalidate_pointers();

	protected:
	wpl_value *create_bool(bool value);
	int flags;

	public:
	wpl_value(const wpl_value &copy) :
		wpl_suicidal(copy),
		flags(0)
	{}
	wpl_value() :
		wpl_suicidal(),
		flags(0)
	{}
	virtual ~wpl_value();
	virtual void suicide() override;

	virtual void register_pointer(wpl_pointer *ptr);
	virtual void remove_pointer(wpl_pointer *ptr); 

	virtual void set_flags(int flags) {
		this->flags = flags;
	}
	virtual int get_flags() {
		return flags;
	}
	virtual void set_do_finalize() {
		this->flags |= wpl_value_do_finalize;
	}

	virtual const char *get_type_name() const = 0;
	virtual int get_precedence() const = 0;
	virtual const wpl_type_complete *get_type() const {
		throw runtime_error("No type exists for this value");
	}

	virtual wpl_value *clone() const = 0;
	virtual wpl_value *clone_empty() const {  throw runtime_error ("Empty cloning not supported by this type"); }

	virtual wpl_value *dereference() {
		return this;
	}

	/* Used by MySQL */
	virtual void resize(int length) {
		throw runtime_error ("Cannot resize value of this type");
	}

	virtual void reset() {
		// Do nothing as default. Structs and arrays override this.
	}

	virtual int do_fastop (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const wpl_operator_struct *op
	);

	virtual int do_operator_discard (
			wpl_expression_state *exp_state,
			wpl_value *discarded,
			wpl_value *final_result
	);

	virtual int do_operator_recursive (
			wpl_expression_state *exp_state,
			wpl_value *final_result
	);

	virtual int do_regex (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const struct wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
	);

	virtual int do_range_operator(
			wpl_expression_state *exp_state,
			const struct wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
	) {
		throw runtime_error("Range operator .. and ... not supported here");
	}

	virtual bool do_pattern_match(
			wpl_expression_state *exp_state,
			string &subject,
			list<unique_ptr<wpl_value>> &matches
	) {
		cerr << "Attempted to match =~ or !~ with type '" << get_type_name() << 
		       "' which is not a /regular expression/\n";
		throw runtime_error("Cannot do pattern match operator");
	}

	virtual int do_operator (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const struct wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
	) {
		return WPL_OP_UNKNOWN;
	}

/*	virtual wpl_value *resolve (wpl_namespace_session *nss) {
		return this;
	}*/

	virtual void output_json(wpl_io &io) {
		cerr << "In value output_json() of type '" << get_type_name() << "':\n";
		throw runtime_error ("Cannot output JSON of this type");
	}
	virtual void output(wpl_io &io) {
		cerr << "In value output() of type '" << get_type_name() << "':\n";
		throw runtime_error ("Cannot output this type");
	}

//	virtual wpl_value *next() { throw runtime_error ("Cannot iterate with next() on this type"); }
/*	virtual void push(wpl_value *value) {
		unique_ptr<wpl_value> value_ptr(value);
		set_weak (value);
	}*/

	virtual int toInt() {
		cerr << "In value toInt() of type '" << get_type_name() << "':\n";
		throw runtime_error ("Cannot get int value of this type");
	}
	virtual bool toBool() {
		cerr << "In value toBool() of type '" << get_type_name() << "':\n";
		throw runtime_error ("Cannot get bool value of this type");
	}
	virtual string toString() const {
		cerr << "In value toString() of type '" << get_type_name() << "':\n";
		throw runtime_error ("Cannot get string value of this type");
	}
	/* Objects which do not support toString() should override this */
	virtual string toStringDBG() {
		return toString();
	}
	virtual char *toVoid() { throw runtime_error ("Cannot get void pointer of this type"); }
	virtual unsigned int toUint() { throw runtime_error ("Cannot get uint value of this type"); }
	virtual long long int toLongLongInt() { throw runtime_error ("Cannot get llint value of this type");}
	virtual unsigned long long int toLongLongUint() { throw runtime_error ("Cannot get lluint value of this type"); }
	virtual float toFloat() { throw runtime_error ("Cannot get float value of this type"); }
	virtual double toDouble() { throw runtime_error ("Cannot get double value of this type"); }

	virtual void dump() {
		cout << " " << toStringDBG();
	}

	virtual bool isStruct() {
		return false;
	}

	virtual bool isArray() {
		return false;
	}

	virtual bool isPointer() {
		return false;
	}

/*	virtual bool isUnresolved() const {
		return false;
	}*/

	virtual int finalize_expression(wpl_expression_state *exp_state, wpl_value *last_value) {
		if (flags & wpl_value_do_finalize) {
			set_weak(last_value);
		}
		return WPL_OP_OK;
	}

	virtual void set_weak (wpl_value *value) {
		throw wpl_value_no_weak_set();
	}

	virtual bool set_strong (wpl_value *value) {
		throw wpl_value_no_strong_set();
	}

	virtual void notify_destructor(wpl_state *state, wpl_namespace_session *nss, wpl_io &io) {
		// Default is no behaviour
	}
};

class wpl_value_template : public wpl_value {
	protected:
	const wpl_type_complete *container_type;
	const wpl_type_complete *template_type;

	shared_ptr<const wpl_type_complete> temporary_type;

	public:
	virtual ~wpl_value_template() {};
	wpl_value_template(
			const wpl_type_complete *container_type,
			const wpl_type_complete *template_type
	) : wpl_value() {
		this->container_type = container_type;
		this->template_type = template_type;
	}

	wpl_value_template(
			wpl_namespace_session *nss,
			shared_ptr<const wpl_type_complete> mother_type,
			const wpl_type_complete *template_type
	);

	const wpl_type_complete *get_container() const {
		return container_type;
	}
	const wpl_type_complete *get_template() const {
		return template_type;
	}
	virtual bool isTemplate() {
		return true;
	}
};
