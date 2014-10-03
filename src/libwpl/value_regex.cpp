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

#include "string.h"
#include "value_regex.h"
#include "value_string.h"
#include "operator_return_values.h"
#include "wpl_string.h"

void wpl_value_regex_rope::element_variable::get (
		wpl_namespace_session *nss,
		boost::match_results<std::string::const_iterator> &what,
		string &target
		) const
{
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

void wpl_value_regex_rope::element_match_index::get (
		wpl_namespace_session *nss,
		boost::match_results<std::string::const_iterator> &what,
		string &target
		) const
{
	if (index >= what.size() || index < 0) {
		ostringstream msg;
		msg << "Regex match replacement '$" << index << "' out of range\n";
		throw runtime_error(msg.str());
	}
	target.append(string(what[index].first, what[index].second));
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

void wpl_value_regex_rope::rope::add_mindex(const char *text) {
	elements.emplace_back(new element_match_index(strtoll(text, NULL, 10)));
}

void wpl_value_regex_rope::rope::get (
		wpl_namespace_session *nss,
		boost::match_results<std::string::const_iterator> &what,
		string &target
) {
	for (const shared_ptr<element> &ptr : elements) {
		ptr->get(nss, what, target);
	}
}

void wpl_value_regex::parse_prefix() {
	const char *pos;
	const char *end;

	int len = search (LOWERCASE_LETTER, 0, false);
	if (len == 0)
		return;

	for (pos = get_string_pointer(), end = get_string_pointer() + len; pos < end; pos++) {
		switch (*pos) {
			case 's':
				do_replace = true;
				break;
			default:
				ostringstream msg;
				msg << "Unknown regex prefix modifier '" << *pos << "'\n";
				THROW_ELEMENT_EXCEPTION(msg.str());
		}
	}
	ignore_string (len);
}

void wpl_value_regex::parse_postfix() {
	const char *pos;
	const char *end;

	int len = search (LOWERCASE_LETTER, 0, false);
	if (len == 0)
		return;

	for (pos = get_string_pointer(), end = get_string_pointer() + len; pos < end; pos++) {
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
				THROW_ELEMENT_EXCEPTION(msg.str());
		}
	}
	ignore_string(len);
}

void wpl_value_regex::parse_main() {
	wpl_matcher_position start = get_position();

	string tmp;
	char letter;
	bool found_end = false;
	bool in_escape_seq = false;
	bool in_variable_name = false;
	while (!at_end() && (letter = get_letter())) {
		if (in_escape_seq) {
			tmp += letter;
			in_escape_seq = false;
		}
		else if (letter == '/') {
			found_end = true;
			break;
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
					revert_string(1);
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

	if (!found_end) {
		load_position(start);
		THROW_ELEMENT_EXCEPTION("Runaway regular expression, missing terminating /");
	}
	else if (in_variable_name) {
		add_var(tmp.c_str());
	}
	else if (in_escape_seq) {
		load_position(start);
		THROW_ELEMENT_EXCEPTION("Runaway escape-sequence '\\' at end of regular expression");
	}
	else {
		elements.add_text(tmp.c_str());
	}
}

void wpl_value_regex::parse_replacement() {
	wpl_matcher_position start = get_position();

	string tmp;
	char letter;
	char word[WPL_VARNAME_SIZE];

	while (!at_end() && (letter = get_letter())) {
		if (letter == '/') {
			replacement_elements.add_text(tmp.c_str());
			return;
		}
		else if (letter == '\\') {
			if (at_end()) {
				load_position(start);
				THROW_ELEMENT_EXCEPTION("Runaway escape-sequence '\\'"
						" at end of regular expression replacement"
						);
			}

			if (!wpl_string_parse_double_escape (&letter, get_letter())) {
				revert_string(1);
				ostringstream msg;
				msg << "Unknown escape sequence '\\" << get_letter() <<
					"' in regex replacement";
				THROW_ELEMENT_EXCEPTION(msg.str());
			}
			tmp += letter;
		}
		else if (letter == '$') {
			replacement_elements.add_text(tmp.c_str());
			tmp = "";

			if (int len = search (NUMBER, 0, false)) {
				check_varname_length(len);
				get_string(word, len);
				replacement_elements.add_mindex(word);
			}
			else if (int len = search (WORD, 0, false)) {
				check_varname_length(len);
				get_string(word, len);
				replacement_elements.add_var(word);
			}
			else {
				THROW_ELEMENT_EXCEPTION("Excepted number or word after '$'"
						" in regular expression replacement"
						);
			}
		}
		else {
			tmp += letter;
		}
	}

	load_position(start);
	THROW_ELEMENT_EXCEPTION("Runaway regular expression replacement, missing terminating /");
}

void wpl_value_regex::parse_value() {
	wpl_matcher_position start = get_position();

	parse_prefix();

	if (!ignore_letter('/')) {
		THROW_ELEMENT_EXCEPTION("Expected regex start / after initial prefix modifiers");
	}

	parse_main();

	if (do_replace) {
		parse_replacement();
	}

	parse_postfix();
}

bool wpl_value_regex::do_pattern_match (
		wpl_expression_state *exp_state,
		string &subject,
		list<unique_ptr<wpl_value>> &matches
) {
	boost::match_flag_type flags = boost::match_default;
	boost::match_results<std::string::const_iterator> what; 
	string::const_iterator start, end;
	/*
	   TODO
	   Implement wpl_regex_state to cache the regex
	 */

	string regex_string;
	elements.get(exp_state->get_nss(), what, regex_string);
	boost::regex my_regex(regex_string);

	bool result = false;

	string replacement;

	start = subject.begin();
	end = subject.end();

	while (regex_search(start, end, what, my_regex, flags)) {
		result = true;

		for (int i = 0; i < what.size(); i++) {
			matches.emplace_back(new wpl_value_string(string(what[i].first, what[i].second)));
		}

		if (do_replace) {
			replacement.append(start, what[0].first);
			replacement_elements.get(exp_state->get_nss(), what, replacement);
		}

		start = what[0].second;

		if (!do_global) {
			break;
		}

		flags |= boost::match_prev_avail; 
		flags |= boost::match_not_bob;
	}

	if (do_replace) {
		replacement.append(start, end);
		subject = replacement;
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


