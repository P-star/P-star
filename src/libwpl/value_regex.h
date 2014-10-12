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
#include "value_holder.h"
#include "exception.h"
#include "matcher.h"

#include <list>
#include <boost/regex.hpp>

struct wpl_operator_struct;

namespace wpl_value_regex_rope {
	using namespace std;

	class element {
		public:
		virtual void get (
				wpl_namespace_session *nss,
				boost::match_results<std::string::const_iterator> &what,
				string &target
		) const = 0;
	};
	class element_variable : public element {
		private:
		string name;
		virtual void hook(string &value) const {} // Do nothing

		public:
		element_variable (const char *_name) :
			name(_name)
		{}
		void get (
				wpl_namespace_session *nss,
				boost::match_results<std::string::const_iterator> &what,
				string &target
		) const override;
	};
	class element_variable_quoted : public element_variable {
		private:
		void hook (string &value) const override;

		public:
		element_variable_quoted (const char *_name) :
			element_variable(_name)
		{}
	};
	class element_text : public element {
		string text;

		public:
		element_text(const char *_text) :
			text(_text)
		{}
		void get (
				wpl_namespace_session *nss,
				boost::match_results<std::string::const_iterator> &what,
				string &target
		) const override {
			target.append(text);
		}
	};
	class element_match_index : public element {
		int index;

		public:
		element_match_index (int i) : index(i) {}
		void get (
				wpl_namespace_session *nss,
				boost::match_results<std::string::const_iterator> &what,
				string &target
		) const override;
	};
	class rope {
		private:
		list<shared_ptr<element>> elements;
		string tmp;

		public:
		void add_var (const char *name);
		void add_qvar (const char *name);
		void add_text (const char *text);
		void add_mindex (const char *text);
		void get (
				wpl_namespace_session *nss,
				boost::match_results<std::string::const_iterator> &what,
				string &target
		);
	};
}	

class wpl_value_regex : public wpl_value, public wpl_matcher {
	private:

	wpl_value_regex_rope::rope elements;
	wpl_value_regex_rope::rope replacement_elements;

	bool quote_inline_variables;
	bool do_global;
	bool do_replace;

	void add_var (const char *name) {
		if (quote_inline_variables) {
			elements.add_qvar(name);
		}
		else {
			elements.add_var(name);
		}
	}

	void parse_prefix();
	void parse_postfix();
	void parse_main();
	void parse_replacement();

	public:
	PRIMITIVE_TYPEATTR(regex)
	wpl_value_regex() {
		quote_inline_variables = false;
		do_global = false;
		do_replace = false;
	}

	void parse_value();

	wpl_value *clone() const {
		return new wpl_value_regex(*this);
	}

	virtual string toStringDBG() override {
		return "{regex}";
	}

	bool do_pattern_match (
			wpl_expression_state *exp_state,
			string &subject,
			list<unique_ptr<wpl_value>> &matches
			) override;

	int do_operator (
			wpl_expression_state *exp_state,
			wpl_value *final_result,
			const wpl_operator_struct *op,
			wpl_value *lhs,
			wpl_value *rhs
	);
};
