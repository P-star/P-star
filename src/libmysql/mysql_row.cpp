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

#include "mysql_row.h"
#include "mysql_stmt.h"

#include "../libwpl/value_bool.h"

void wpl_value_MYSQL_ROW::init (
		shared_ptr<wpl_mysql_stmt_holder> &stmt_holder,
		shared_ptr<wpl_mysql_res_holder> &res_holder
		)
{
	mysql_stmt = stmt_holder;
	mysql_res = res_holder;
}

bool wpl_value_MYSQL_ROW::set_strong (wpl_value *value) {
	wpl_value_MYSQL_ROW *src;
	if (!(src = dynamic_cast<wpl_value_MYSQL_ROW*>(value))) {
		return false;
	}
	mysql_stmt = src->get_stmt_shared_ptr();
	mysql_res = src->get_res_shared_ptr();
	return true;
}

wpl_value *wpl_value_MYSQL_ROW::get_column (const string &name) {
	wpl_mysql_field *field = mysql_res->get_field(name);
	if (!field) {
		cerr << "MySQL error while fetching column '" << name << "':\n";
		throw runtime_error("Unknown column name");
	}
	return field->value.get();
}

wpl_value *wpl_value_MYSQL_ROW::get_column (int index) {
	if (index >= mysql_res->num_fields()) {
		cerr << "MySQL error while fetching column with index '" << index << "':\n";
		throw runtime_error("Column index out of range");
	}
	return mysql_res->get_field(index)->value.get();
}

bool wpl_value_MYSQL_ROW::next_row () {
	return mysql_res->fetch();
}

int wpl_value_MYSQL_ROW::do_operator (
		wpl_expression_state *exp_state,
		wpl_value *final_result,
		const struct wpl_operator_struct *op,
		wpl_value *lhs,
		wpl_value *rhs
		)
{
	if (op == &OP_ASSIGN) {
		if (lhs->set_strong(this)) {
			return this->do_operator_recursive(exp_state, final_result);
		}
		cerr << "While assigning value of type " << rhs->get_type_name() <<
			" to value of type " << get_type_name() << " in MySQL:\n";
		throw runtime_error ("Incompatible types");
	}

	if (!get_stmt()) {
		throw runtime_error("MySQL error: No statement is set for MySQL row");
	}
	if (!get_res()) {
		throw runtime_error("MySQL error: No result set for MySQL row");
	}

	int ret = (WPL_OP_NO_RETURN|WPL_OP_UNKNOWN);

	wpl_value *res;

	if ((op == &OP_INC_SUFFIX) || (op == &OP_INC_PREFIX)) {
		wpl_value_bool my_bool(next_row());

		return my_bool.do_operator_recursive(exp_state, final_result);
	}
	else if ((op == &OP_ARRAY_SUBSCRIPTING) && (rhs->get_precedence() == wpl_type_precedence_int)) {
		return get_column(rhs->toInt())->do_operator_recursive(exp_state, final_result);
	}
	else if (op == &OP_ARRAY_SUBSCRIPTING) {
		string column_name = rhs->toString();
		return get_column(column_name)->do_operator_recursive(exp_state, final_result);
	}

	return ret;
}

