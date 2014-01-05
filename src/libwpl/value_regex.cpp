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

#include "matcher.h"
#include "value_regex.h"
#include "value_string.h"
#include "operator_return_values.h"

void wpl_value_regex_rope::element_variable::get(wpl_namespace_session *nss, string &target) const {
	wpl_variable *variable = nss->find_variable(name.c_str(), WPL_NSS_CTX_SELF);
	if (!variable) {
		ostringstream msg;
		msg << "Could not find variable '" << name << "' in regular expression\n";
		throw runtime_error(msg.str());
	}
	string tmp = variable->get_value()->toString();
	hook (tmp);
	target += tmp;
}

void wpl_value_regex_rope::element_variable_quoted::hook(string &value) const {
	value.insert(0, "\\Q");
	value.append("\\E");
}

void wpl_value_regex_rope::rope::add_qvar (const char *name) {
	if (strlen (name) == 0) {
		return;
	}
	elements.emplace_back(new element_variable_quoted(name));
}

void wpl_value_regex_rope::rope::add_var (const char *name) {
	if (strlen (name) == 0) {
		return;
	}
	elements.emplace_back(new element_variable(name));
}

void wpl_value_regex_rope::rope::add_text (const char *text) {
	if (strlen (text) == 0) {
		return;
	}
	elements.emplace_back(new element_text(text));
}

void wpl_value_regex_rope::rope::get (wpl_namespace_session *nss, string &target) {
	for (const shared_ptr<element> &ptr : elements) {
		ptr->get(nss, target);
	}
}

wpl_value_regex::wpl_value_regex(const char *regex_string, const char *prefix, const char *postfix) :
	elements()
{
	quote_inline_variables = false;
	do_global = false;

	const char *pos;
	const char *end;
	for (pos = prefix, end = prefix + strlen(prefix); pos < end; pos++) {
		switch (*pos) {
			default:
				ostringstream msg;
				msg << "Unknown regex prefix modifier '" << *pos << "'\n";
				throw runtime_error(msg.str());
		}
	}
	for (pos = postfix, end = postfix + strlen(postfix); pos < end; pos++) {
		switch (*pos) {
			case 'q':
				quote_inline_variables = true;
				break;
			case 'g':
				do_global = true;
				break;
			default:
				ostringstream msg;
				msg << "Unknown regex postfix modifier '" << *pos << "'\n";
				throw runtime_error(msg.str());
		}
	}
	

	wpl_matcher matcher(regex_string, strlen(regex_string), "{REGEX}");

	string tmp;
	char letter;
	bool in_escape_seq = false;
	bool in_variable_name = false;
	while (!matcher.at_end() && (letter = matcher.get_letter())) {
		if (in_escape_seq) {
			tmp += letter;
			in_escape_seq = false;
		}
		else if (in_variable_name) {
			if (M_WORD(letter)) {
				tmp += letter;
			}
			else {
				if (tmp.size() == 0) {
					// We add the dollar sign also
					tmp += '$';
					tmp += letter;
				}
				else {
					add_var(tmp.c_str());
					tmp = "";
					matcher.revert_string(1);
				}
				in_variable_name = false;
			}
		}
		else if (letter == '\\') {
			tmp += letter;
			in_escape_seq = true;
		}
		else if (letter == '$') {
			elements.add_text(tmp.c_str());
			tmp = "";
			in_variable_name = true;
		}
		else {
			tmp += letter;
		}
	}
	if (in_variable_name) {
		add_var(tmp.c_str());
	}
	else if (in_escape_seq) {
		throw runtime_error("Runaway escape-sequence '\\' at end of regular expression");
	}
	else {
		elements.add_text(tmp.c_str());
	}
}

bool wpl_value_regex::do_pattern_match (
		wpl_expression_state *exp_state,
		string &subject,
		list<unique_ptr<wpl_value>> &matches
) {
	/*
	   TODO
	   Implement wpl_regex_state to cache the regex
	 */
	/*
	   TODO
	   - add /g-modifier for multiple match
	   - push matches to discard queue
	   */

	string regex_string;
	elements.get(exp_state->get_nss(), regex_string);
	boost::regex my_regex(regex_string);

	bool result = false;

	boost::match_results<std::string::const_iterator> what; 
	boost::match_flag_type flags = boost::match_default;
	string::const_iterator start, end;

	start = subject.begin();
	end = subject.end();

	while (regex_search(start, end, what, my_regex, flags)) {
		result = true;

		for (int i = 0; i < what.size(); i++) {
			string match(what[i].first, what[i].second);
			matches.emplace_back(new wpl_value_string(string(what[i].first, what[i].second)));
		}

		if (!do_global) {
			break;
		}

		start = what[0].second;
		flags |= boost::match_prev_avail; 
		flags |= boost::match_not_bob;
	}

	return result;
}

int wpl_value_regex::do_operator (
		wpl_expression_state *exp_state,
		wpl_value *final_result,
		const wpl_operator_struct *op,
		wpl_value *lhs,
		wpl_value *rhs
		) {

	list<string> matches;

	return WPL_OP_UNKNOWN;
}


