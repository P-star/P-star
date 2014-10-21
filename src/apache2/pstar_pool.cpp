/*
   This file is part of P-star (http://www.p-star.org), and
   is licenced to you under the Apache Licence, Version 2.0.
   You should have received a copy of the licence with this
   software, but it can also be obtained here:

      http://www.apache.org/licenses/LICENSE-2.0
   
   Parts this file is obtained from code owned partly or completely
   by the Apache Software Foundation (ASF). Under follows a
   list of copyright owners, refer to the Apache Web Server project
   for a complete complete list.
   
   - Copyright MMXIII Atle Solbakken
      http://www.p-star.org
      atle@goliathdns.no 
   - Copyright 2012 Apache Software Foundation
      http://www.apache.org/
   
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "pstar_pool.h"
#include "pstar_io.h"
#include "namespace_session.h"

#include "http_log.h"
#include "http_protocol.h"
#include "util_script.h"
#include "apr_lib.h"
#include "apr_strings.h"

#include "apache_24.h"

#include <cstring>

pstar_file::pstar_file (wpl_io &io, const char *filename, int mtime) :
	program(io, 0, NULL)
{
	this->mtime = mtime;
	program.parse_file(filename);
}

int fill_input (request_rec *r, pstar_io &io) {
	/*
	   Credits:
	   Some of this code is from Apaches mod_cgi.c
	   */

	int total_length = 0;
	int seen_eos = 0;
	apr_bucket_brigade *bb;
	apr_status_t rv;

	bb = apr_brigade_create(r->pool, r->connection->bucket_alloc);
	do {
		apr_bucket *bucket;

		rv = ap_get_brigade(r->input_filters, bb, AP_MODE_READBYTES,
				APR_BLOCK_READ, HUGE_STRING_LEN);

		if (rv != APR_SUCCESS) {
			ap_log_rerror(APLOG_MARK, APLOG_ERR, rv, r,
					"Error reading request entity data");
			return -1;
		}

		for (bucket = APR_BRIGADE_FIRST(bb);
				bucket != APR_BRIGADE_SENTINEL(bb);
				bucket = APR_BUCKET_NEXT(bucket))
		{
			const char *data;
			apr_size_t len;

			if (APR_BUCKET_IS_EOS(bucket)) {
				seen_eos = 1;
				break;
			}

			/* We can't do much with this. */
			if (APR_BUCKET_IS_FLUSH(bucket)) {
				continue;
			}

			/* read */
			apr_bucket_read(bucket, &data, &len, APR_BLOCK_READ);

			io.append_input(data, len);
			total_length += len;
		}
		apr_brigade_cleanup(bb);
	}
	while (!seen_eos);

	apr_brigade_cleanup(bb);

	return total_length;
}

shared_ptr<pstar_file> pstar_pool::get_file_handle (
		request_rec *r,
		wpl_io &io,
		const char *filename,
		int mtime
		)
{
	pstar_map_t::iterator it;

	lock_guard<mutex> lock(pool_lock);

	// Check if a cached program exists
	it = files.find(filename);
	if (it != files.end()) {
		if (it->second->is_modified (mtime)) {
			files.erase(it);
		}
		else {
			return it->second;
		}
	}

	shared_ptr<pstar_file> file_ptr(new pstar_file(io, filename, mtime));

	files.insert(std::pair<string,shared_ptr<pstar_file>>(filename, file_ptr));

	return file_ptr;
}

int pstar_pool::handle_file (request_rec *r, const char *filename, int mtime) {
	int ret;
	int input_length;

	pstar_io io(r);

	try {
		try {
			shared_ptr<pstar_file> file = get_file_handle(r, io, filename, mtime);
			wpl_program *program = file->get_program();

			if ((input_length = fill_input(r, io)) < 0) {
				return HTTP_INTERNAL_SERVER_ERROR;
			}


			ret = program->run(io);
		}
		catch (const wpl_parser_exception &e) {
			ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
					"P* Parse error in file '%s': %s", r->filename, e.what().c_str());
			return HTTP_INTERNAL_SERVER_ERROR;
		}
	}
	catch (const wpl_element_exception &e) {
		ostringstream msg;
		msg << "P* exception in file '" << r->filename << "':";
		io.error(msg.str().c_str());
		e.output(io);
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	if (ret != 0) {
		ostringstream msg;
		msg << "P* Non-zero return (" << ret << ") from script " << r->filename;
		io.error(msg.str().c_str());
		return HTTP_INTERNAL_SERVER_ERROR;
/*		ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
				"P* Non-zero return (%i) from script %s", ret, r->filename);
		return HTTP_INTERNAL_SERVER_ERROR;*/
	}

	if (ret != OK) {
		ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
				"P* error (s %i/u %u) when parsing output from script %s", ret, ret, r->filename);
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	return OK;
}

int pstar_pool::handle_request (request_rec *r) {
//	ap_set_content_type(r, "text/html");
	const char *filename;

	filename = r->filename;//apr_filepath_name_get(r->filename);
	if (r->finfo.filetype == 0)
		return HTTP_NOT_FOUND;
	if (r->finfo.filetype == APR_DIR)
		return HTTP_FORBIDDEN;
	if ((r->used_path_info == AP_REQ_REJECT_PATH_INFO) &&
		r->path_info && *r->path_info
	) {
		return HTTP_NOT_FOUND;
	}

	ap_add_common_vars(r);
	ap_add_cgi_vars(r);

	apr_table_t *e = r->subprocess_env;

	if (r->args) {
		apr_table_setn(e, "QUERY_STRING", r->args);
	}

	// Remove filename from path
	char *path = apr_pstrdup(r->pool, filename);
	char *pos = strrchr(path, '/');

	if (!pos) {
		ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
			"P* did not understand this filename: '%s'", filename);
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	*pos = '\0';

	apr_table_setn(e, "PSTAR_ROOT", path);

	return handle_file(r, filename, r->finfo.mtime);
}
