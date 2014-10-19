/*

-------------------------------------------------------------

Copyright (c) MMXIV Atle Solbakken
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

#include "value_holder_macros.h"
#include "value.h"

#include <memory>

class wpl_operator_struct;

/**
 * @brief The auto value holds another value of any type and forwards all functions to it
 */
class wpl_value_auto : public wpl_value {
	private:
	shared_ptr<wpl_value> my_value;

	inline bool set_if_empty(wpl_value *value) {
		if (!my_value.get()) {
			my_value.reset(value->clone());
			my_value->set_flags(flags);
			return true;
		}
		return false;
	}

	inline void check_empty() {
		if (!my_value.get()) {
			throw runtime_error("Attempted to use auto-type before it was set to a value");
		}
	}

	public:
	wpl_value_auto () : wpl_value() {}
	wpl_value_auto (int dummy) {}

	void register_pointer(wpl_pointer *ptr) override {
		check_empty();
		my_value->register_pointer(ptr);
	}
	void remove_pointer(wpl_pointer *ptr) override {
		check_empty();
		my_value->remove_pointer(ptr);
	}

	void set_flags(int flags) override {
		wpl_value::set_flags(flags);
		if (my_value.get()) {
			my_value->set_flags(flags);
		}
	}
	int get_flags() override {
		if (my_value.get()) {
			return my_value->get_flags();
		}
		return wpl_value::get_flags();
	}
	void set_do_finalize() override {
		if (my_value.get()) {
			my_value->set_do_finalize();
		}
		else {
			wpl_value::set_do_finalize();
		}
	}


	const char *get_type_name() const override {
		return my_value.get() ? my_value->get_type_name() : wpl_typename_auto;
	}
	int get_precedence() const override {
		return my_value.get() ? my_value->get_precedence() : wpl_type_precedence_auto;
	}
	const wpl_type_complete *get_type() const override {
		return my_value.get() ? my_value->get_type() : wpl_type_global_auto;
	}

	/* When cloning, we can optimize ourselves away */
	wpl_value *clone() const override {
		return my_value.get() ? my_value->clone() : new wpl_value_auto(*this);
	}
	wpl_value *clone_empty() const override {
		return my_value.get() ? my_value->clone_empty() : new wpl_value_auto();
	}

	wpl_value *dereference() override {
		check_empty();
		return my_value->dereference();
	}

	void resize(int length) override {
		check_empty();
		my_value->resize(length);
	}

	void reset() override {
		check_empty();
		my_value->reset();
	}

	int do_fastop (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const wpl_operator_struct *op
	) override {
		check_empty();
		return my_value->do_fastop(exp_state, final_result, op);
	}

	int do_operator_discard (
			wpl_expression_state *exp_state,
			wpl_value *discarded,
			wpl_value *final_result
	) override {
		check_empty();
		return my_value->do_operator_discard(exp_state, discarded, final_result);
	}

	int do_operator_recursive(wpl_expression_state *exp_state, wpl_value *final_result) {
		/* At first run, we use our own do_operator_recursive, hoping 
		   that the expression will assign to us using set_weak later */
		if (my_value.get()) {
			return my_value->do_operator_recursive(exp_state, final_result);
		}
		else {
			return wpl_value::do_operator_recursive(exp_state, final_result);
		}
	}

	int do_regex (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const struct wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
	) override {
		check_empty();
		return my_value->do_regex(exp_state, final_result, op, lhs, rhs);
	}

	int do_range_operator(
			wpl_expression_state *exp_state,
			const struct wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
	) override {
		check_empty();
		return my_value->do_range_operator(exp_state, op, lhs, rhs);
	}

	bool do_pattern_match(
			wpl_expression_state *exp_state,
			string &subject,
			list<unique_ptr<wpl_value>> &matches
	) override {
		check_empty();
		return my_value->do_pattern_match(exp_state, subject, matches);
	}

	int do_operator (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
	) override {
		check_empty();
		return my_value->do_operator(exp_state, final_result, op, lhs, rhs);
	}

	void output_json(wpl_io &io) override {
		check_empty();
		my_value->output_json(io);
	}

	void output(wpl_io &io) override {
		check_empty();
		my_value->output(io);
	}

	int toInt() override {
		return my_value.get() ? my_value->toInt() : 0;
	}
	bool toBool() override {
		return my_value.get() ? my_value->toBool() : false;
	}
	string toString() const override {
		return my_value.get() ? my_value->toString() : "";
	}
	string toStringDBG() override {
		string ret;
		if (my_value.get()) {
			ret = "{auto{" + my_value->toStringDBG() + "}}";
		}
		else {
			ret = "{auto{unset}}";
		}
		return ret;
	}
	char *toVoid() override {
		return my_value.get() ? my_value->toVoid() : wpl_value::toVoid();
	}
	unsigned int toUint() override {
		return my_value.get() ? my_value->toUint() : 0;
	}
	long long int toLongLongInt() override {
		return my_value.get() ? my_value->toLongLongInt() : 0;
	}
	unsigned long long int toLongLongUint() {
		return my_value.get() ? my_value->toLongLongUint() : 0;
	}
	float toFloat() override {
		return my_value.get() ? my_value->toFloat() : 0.0;
	}
	double toDouble() override {
		return my_value.get() ? my_value->toDouble() : 0.0;
	}

	void dump() override {
		cout << " " << toStringDBG();
	}

	bool isStruct() override {
		return my_value.get() ? my_value->isStruct() : false;
	}

	bool isArray() override {
		return my_value.get() ? my_value->isArray() : false;
	}

/*	bool isUnresolved() override {
		check_empty();
		return my_value->isUnresolved();
	}*/

	int finalize_expression(wpl_expression_state *exp_state, wpl_value *last_value) override {
		if (my_value.get()) {
			return my_value->finalize_expression(exp_state, last_value);
		}
		else {
			set_if_empty(last_value);
			last_value->set_do_finalize();
			return WPL_OP_OK;
		}
	}

	void set_weak(wpl_value *value) override {
		if (!set_if_empty(value)) {
			my_value->set_weak(value);
		}
	}

	bool set_strong(wpl_value *value) override {
		if (!set_if_empty(value)) {
			return my_value->set_strong(value);
		}
		return true;
	}

	void notify_destructor(wpl_state *state, wpl_namespace_session *nss, wpl_io &io) override {
		check_empty();
		my_value->notify_destructor(state, nss, io);
	}
};
