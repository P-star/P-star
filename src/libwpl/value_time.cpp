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

#include "value_time.h"
#include "operator.h"
#include "value_int.h"
#include "value_string.h"

#include <ctime>
#include <cstring>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

void wpl_value_time::set_weak (wpl_value *value) {
	/*
	   TODO
	   Support setting from string and int
	   */
	wpl_value_time *value_time = dynamic_cast<wpl_value_time*>(value);
	if (value_time == NULL) {
        wpl_value_string * value_string = dynamic_cast<wpl_value_string*>(value);
        if (value_string) {
            try_guess_from_str(value_string->toString());
        } else {
            ostringstream tmp;
            tmp << "Could not set TIME object to value of type " << value->get_type_name();
            throw runtime_error(tmp.str());
        }
	}
	else {
		set_time(value_time->get_time());
		set_format(value_time->get_format());
	}
}

string wpl_value_time::toString() {
	string tmp;
	format_time(NULL, tmp);
	return tmp;
}

int wpl_value_time::do_operator (
		wpl_expression_state *exp_state,
		wpl_value *final_result,
		const wpl_operator_struct *op,
		wpl_value *lhs,
		wpl_value *rhs
) {
	if (op == &OP_ELEMENT) {
		const char *cmd = rhs->toString().c_str();

		/*
		   Commands which return self
		 */
		if (strcmp (cmd, "set_now") == 0) {
			set_now();
			return do_operator_recursive(exp_state, final_result);
		}

		/*
		   Commands which return strings
		 */
		{
			wpl_value_string result("");
			bool ok = true;

			if (strcmp (cmd, "rfc2822") == 0) {
				format_time("%a, %d %b %Y %T %z", result.get_internal_string());
			}
			else if (strcmp (cmd, "iso8601") == 0) {
				format_time("%Y-%m-%dT%H:%M:%S%z", result.get_internal_string());
			}
			else {
				ok = false;
			}

			if (ok) {
				return result.do_operator_recursive(exp_state, final_result);
			}
		}

		/*
		   Commands which return integers
		 */
		{
			time_t t = get_time();
			struct tm time_tm;
			localtime_r(&t, &time_tm);

			wpl_value_int result(0);
			bool ok = true;

			if (strcmp (cmd, "year") == 0) {
				result.set(time_tm.tm_year + 1900);
			}
			else if (
				(strcmp (cmd, "month") == 0) ||
				(strcmp (cmd, "mon") == 0)
			) {
				result.set(time_tm.tm_mon + 1);
			}
			else if (strcmp (cmd, "day") == 0) {
				result.set(time_tm.tm_mday);
			}
			else if (strcmp (cmd, "hour") == 0) {
				result.set(time_tm.tm_hour);
			}
			else if (strcmp (cmd, "min") == 0) {
				result.set(time_tm.tm_min);
			}
			else if (strcmp (cmd, "sec") == 0) {
				result.set(time_tm.tm_sec);
			}
			else {
				ok = false;
			}

			if (ok) {
				return result.do_operator_recursive(exp_state, final_result);
			}
		}

		ostringstream tmp;
		tmp << "Member '" << cmd << "' is uknown for TIME objects";
		throw runtime_error(tmp.str());
	}
	else if (op == &OP_CONCAT) {
		wpl_value_string result("");
		string &tmp = result.get_internal_string();

		format_time(NULL, tmp);
		tmp += rhs->toString();

		return result.do_operator_recursive(exp_state, final_result);
	}
	else if (op == &OP_ARRAY_SUBSCRIPTING) {
		wpl_value_string result("");
	
		format_time(rhs->toString().c_str(), result.get_internal_string());
	
		return result.do_operator_recursive(exp_state, final_result);
	}
	else if (op == &OP_ASSIGN) {
		set_weak(rhs);
		return do_operator_recursive(exp_state, final_result);
	}
	else if (op == &OP_FUNCTION_CALL) {
		throw runtime_error("Unexpected function call () after TIME object");
	}

	return WPL_OP_UNKNOWN;
}

static inline
string try_guess_sep(const std::string& s){
    string sep;
    std::for_each (s.begin(),
                   s.end(),
                   [&sep](char c){
                    if (!isdigit(c)){
                        if (sep.empty())
                            sep = c;
                        else if (sep[0] != c)
                            throw runtime_error("Invalid date/time format (try with one type of separator)");
                    }
                   }
    );
    return sep;
}

static inline
void replace_month_name(string& fmt){
    static const std::string long_names[] = {"january", "february", "march", "april",
                                           "may", "june", "july","august",
                                           "september","october","november","december"};
    for (int i=0 ; i<sizeof(long_names)/sizeof(long_names[0]) ; ++i){
        boost::replace_all(fmt, long_names[i], boost::lexical_cast<string>(i+1));
        boost::replace_all(fmt, long_names[i].substr(0,3), boost::lexical_cast<string>(i+1));
    }
}

void wpl_value_time::try_guess_from_str(const string &fmt_){
    string fmt = fmt_;
    replace_month_name(fmt);
    const string sep = try_guess_sep(fmt);
    std::vector<std::string> parts;
    boost::split(parts, fmt, boost::is_any_of(sep.c_str()), boost::token_compress_on);
    int year=0;
    int month=0;
    int day=0;
    std::string guessed_fmt;
    std::for_each(parts.begin(),
                  parts.end(),
                  [&year,&month,&day,&guessed_fmt,sep](const std::string& str){
                    try{
                        if (year == 0){
                            year = boost::lexical_cast<int>(str);
                            if (! guessed_fmt.empty())
                                guessed_fmt += sep;
                            guessed_fmt += "%Y";
                        } else if (str.length()==2 || str.length()==1){
                            if (month==0){
                                month = boost::lexical_cast<int>(str);
                                if (! guessed_fmt.empty())
                                    guessed_fmt += sep;
                                guessed_fmt += "%m";
                            } else if (day == 0){
                                day = boost::lexical_cast<int>(str);
                                if (! guessed_fmt.empty())
                                    guessed_fmt += sep;
                                guessed_fmt += "%d";
                            } else {
                                throw runtime_error("Invaild date/time format (too many date tokens)");
                            }
                        } else {
                            throw runtime_error("Invaild date/time format (invalid substring length)");
                        }
                    } catch (const boost::bad_lexical_cast& e){
                        throw runtime_error(e.what());
                    }
                  }
    );
    if (year==0 || month==0 || day==0)
        throw runtime_error("Invalid date/time format: year, month and day must be specified");
    struct tm time_tmp;
    memset(&time_tmp,0,sizeof(time_tmp));
    time_tmp.tm_year = year-1900;
    time_tmp.tm_mon = month-1;
    time_tmp.tm_mday = day;
    this->set_format(guessed_fmt);
    this->set_time(mktime(&time_tmp));
}
