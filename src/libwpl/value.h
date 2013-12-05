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

#include "shunting_yard_carrier.h"
#include "operator_return_values.h"
#include "typenames.h"
#include "type_precedence.h"
#include "exception.h"
#include "suicidal.h"
#include "debug.h"
#include "io.h"

enum {
	wpl_value_no_flags,
	wpl_value_is_constant,
	wpl_value_is_dynamic
};

struct wpl_operator_struct;
class wpl_namespace_session;
class wpl_expression_state;
class wpl_type_complete;
class wpl_value_bool;
class wpl_value;

/**
 * @brief This class should be allocated on the stack before running runables. Set the result from the runable with set(), and the value will be automatically deleted on destruction of this class if WPL_OP_RETURN_REFERENCE is set.
 */
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

/**
 * @brief This is the master value class. All values derive from this type. Values support doing operators and to be converted between one another.
 */
class wpl_value : public wpl_suicidal {
	private:

	protected:
	wpl_value *create_bool(bool value);
	int flags;

	public:
	wpl_value() : wpl_suicidal() {
		flags = 0;
	}
	wpl_value(const wpl_value &copy) : wpl_suicidal(copy) {
		flags = 0;
	}
	virtual ~wpl_value();
	void suicide() override {
		delete this;
	}

	void set_flags(int flags) {
		this->flags = flags;
	}
	int get_flags() {
		return flags;
	}

	virtual const char *get_type_name() const = 0;
	virtual int get_precedence() const = 0;

	virtual wpl_value *clone() const = 0;
	virtual wpl_value *clone_empty() const {  throw runtime_error ("Empty cloning not supported by this type"); }

	virtual wpl_value *dereference() {
		return this;
	}

	virtual void resize(int length) {
		throw runtime_error ("Cannot resize value of this type");
	}

	virtual int do_fastop (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const wpl_operator_struct *op
	);

	virtual int do_operator_recursive (
			wpl_expression_state *exp_state,
			wpl_value *final_result
	);

	int do_regex (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const struct wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
	);

	virtual bool do_pattern_match(wpl_expression_state *exp_state, string &subject) {
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

	virtual wpl_value *resolve (wpl_namespace_session *nss) {
		return this;
	}

	virtual void output_json(wpl_io &io) {
		cerr << "In value output_json() of type '" << get_type_name() << "':\n";
		throw runtime_error ("Cannot output JSON of this type");
	}
	virtual void output(wpl_io &io) {
		cerr << "In value output() of type '" << get_type_name() << "':\n";
		throw runtime_error ("Cannot output this type");
	}

	virtual wpl_value *next() { throw runtime_error ("Cannot iterate with next() on this type"); }
/*	virtual void push(wpl_value *value) {
		unique_ptr<wpl_value> value_ptr(value);
		set_weak (value);
	}*/

	virtual int toInt() {
		return 0;
		/*
		   TODO implement toInt for all types
		   cerr << "In value toInt() of type '" << get_type_name() << "':\n";
		   throw runtime_error ("Cannot get int value of this type");
		 */
	}
	virtual bool toBool() {
		cerr << "In value toBool() of type '" << get_type_name() << "':\n";
		throw runtime_error ("Cannot get bool value of this type");
	}
	virtual string toString() {
		cerr << "In value toString() of type '" << get_type_name() << "':\n";
		throw runtime_error ("Cannot get string value of this type");
	}
	virtual char *toVoid() { throw runtime_error ("Cannot get void pointer of this type"); }
	virtual unsigned int toUint() { throw runtime_error ("Cannot get uint value of this type"); }
	virtual long long int toLongLongInt() { throw runtime_error ("Cannot get llint value of this type");}
	virtual unsigned long long int toLongLongUint() { throw runtime_error ("Cannot get lluint value of this type"); }
	virtual float toFloat() { throw runtime_error ("Cannot get float value of this type"); }
	virtual double toDouble() { throw runtime_error ("Cannot get double value of this type"); }

	virtual void dump() {
		cout << " " << toString();
	}

	virtual bool isStruct() {
		return false;
	}

	virtual bool isArray() {
		return false;
	}

	virtual bool isUnresolved() const {
		return false;
	}

	virtual int finalize_expression(wpl_expression_state *exp_state, wpl_value *last_value) {
		set_weak(last_value);
		return WPL_OP_OK;
	}

	virtual void set_weak (wpl_value *value) {
		cerr << "In value set_weak() of type '" << get_type_name() << "'\n";
		cerr << "while trying to set with type '" << value->get_type_name() << "'\n";
		throw runtime_error("Weak set not supported by this type");
	}

	virtual bool set_strong (wpl_value *value) {
		throw runtime_error("Strong set not supported by this type");
	}
};

class wpl_value_template : public wpl_value {
	protected:
	const wpl_type_complete *template_type;

	public:
	virtual ~wpl_value_template() {};
	wpl_value_template(const wpl_type_complete *template_type) {
		this->template_type = template_type;
	}
	const wpl_type_complete *get_template() {
		return template_type;
	}
	virtual bool isTemplate() {
		return true;
	}
};
