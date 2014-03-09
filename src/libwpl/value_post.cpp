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

#include "value_post.h"
#include "operator.h"
#include "value_string.h"
#include "value_array.h"
#include "matcher.h"

#include <vector>
#include <memory>
#include <cstring>
#include <map>

using namespace mimetic;

extern wpl_type_bool *wpl_type_global_bool;
extern wpl_type_array *wpl_type_global_array;

wpl_type_array_instance wpl_value_post::type_complete_array(
	wpl_type_global_array, wpl_type_global_bool
);

void wpl_value_post::parse_entity (MimeEntity *me) {
	Header &header = me->header();

	string name = header.contentDisposition().param(string("name"));

	if (!name.empty()) {
		string out;

		/* Credits: https://github.com/SteveJones/mailview/blob/master/mailview.cpp */
		if (me->hasField("Content-Transfer-Encoding")) {
			std::string content_transfer_encoding
				= header.contentTransferEncoding().mechanism();
			if (content_transfer_encoding == "quoted-printable") {
				QP::Decoder qp;
				decode(me->body().begin(), me->body().end(), qp, out.begin());
				goto copy_done;
			}
			if (content_transfer_encoding == "base64") {
				Base64::Decoder b64;
				decode(me->body().begin(), me->body().end(), b64, out.begin());
				goto copy_done;
			}
		}

		out = me->body();

		copy_done:
		wpl_value_array *storage = (wpl_value_array*) hash.get(name);
		if (!storage) {
			storage = (wpl_value_array*) hash.define(name);
		}
		storage->push(new wpl_value_string(out));
	}

	/* Credits: http://www.codesink.org/mimetic_mime_library.html */
	MimeEntityList &parts = me->body().parts();
	MimeEntityList::iterator mbit = parts.begin(), meit = parts.end();
	for(; mbit != meit; ++mbit) {
		parse_entity(*mbit);
	}
}

/* 10MB max upload size */
#define MAX_REQUEST_SIZE 10*1024*1024

void wpl_value_post::parse(wpl_io &io) {
	const char *content_type = io.get_env("CONTENT_TYPE");
	const char *content_length = io.get_env("CONTENT_LENGTH");
	const char *request_method = io.get_env("REQUEST_METHOD");

	if (!request_method || strcmp(request_method, "POST") != 0 || !content_type) {
		return;
	}

	if (!content_length) {
		throw runtime_error("Could not find CONTENT_LENGTH environment variable while parsing POST-query");
	}

	long int length = strtol(content_length, NULL, 10);

	if (length <= 0) {
		cerr << "Invalid value for CONTENT_LENGTH '" << content_length << "'\n";
		throw runtime_error("Error while parsing CONTENT_LENGTH environment variable in POST object");
	}

	if (length > MAX_REQUEST_SIZE) {
		throw runtime_error("wpl_value_post::parse(): Exceeded maximum upload size, max is 10MB");
	}

	vector<char> buf_ptr;
	buf_ptr.resize(length+1);
	char *buf = buf_ptr.data();

	io.read(buf, length);
	buf[length] = '\0';

	if (strcmp(content_type, "application/x-www-form-urlencoded") == 0) {
		value_get.parse(buf);

		use_get = true;
		return;
	}

	const char name[] = "multipart/form-data";
	if (strncmp(content_type, name, sizeof(name)-1) == 0) {
		const char *ct = "Content-Type: ";
		stringstream ss;
		ss << ct << content_type << "\r\n\r\n" << buf;

		MimeEntity me(ss);

		parse_entity(&me);

		return;
	}
}

int wpl_value_post::do_operator (
		wpl_expression_state *exp_state,
		wpl_value *final_result,
		const wpl_operator_struct *op,
		wpl_value *lhs,
		wpl_value *rhs
) {
	if (!did_parse) {
		parse(exp_state->get_io());
		did_parse = true;
	}

	if (use_get) {
		return value_get.do_operator(
			exp_state,
			final_result,
			op,
			(lhs == this ? &value_get : lhs),
			(rhs == this ? &value_get : rhs)
		);
	}

	return hash.do_operator (
		exp_state,
		final_result,
		op,
		(lhs == this ? &hash : lhs),
		(rhs == this ? &hash : rhs)
	);
}
