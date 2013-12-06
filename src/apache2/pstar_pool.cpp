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

pstar_file::pstar_file (const char *filename, int mtime) :
	program(0, NULL)
{
	this->mtime = mtime;
	program.parse_file(filename);
}

int fill_input (request_rec *r, pstar_io &io) {
	/*
	   Credits:
	   Some of this code is from Apaches mod_cgi.c
	   */

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
			return HTTP_INTERNAL_SERVER_ERROR;
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
		}
		apr_brigade_cleanup(bb);
	}
	while (!seen_eos);

	apr_brigade_cleanup(bb);

	return OK;
}

pstar_pool::pstar_pool(apr_pool_t *pool) {
	apr_proc_mutex_create (
			&mutex,
			"pstar_pool_mutex",
			APR_LOCK_DEFAULT,
			pool
	);
}

shared_ptr<pstar_file> pstar_pool::get_file_handle (request_rec *r, const char *filename, int mtime) {
	pstar_map_t::iterator it;

	/*
	   TODO
	   Apache may be handling modification-issues itself, check this out
	   */

	apr_proc_mutex_lock (mutex);

	// Check if a cached program exists
	it = files.find(filename);
	if (it != files.end()) {
		if (it->second->is_modified (mtime)) {
			files.erase(it);
		}
		else {
			apr_proc_mutex_unlock (mutex);
			return it->second;
		}
	}

	apr_proc_mutex_unlock (mutex);

	shared_ptr<pstar_file> file_ptr(new pstar_file(filename, mtime));

	apr_proc_mutex_lock (mutex);
	files.insert(std::pair<string,shared_ptr<pstar_file>>(filename, file_ptr));
	apr_proc_mutex_unlock (mutex);

	return file_ptr;
}

int pstar_pool::handle_file (request_rec *r, const char *filename, int mtime) {
	int ret;

	shared_ptr<pstar_file> file = get_file_handle(r, filename, mtime);
	wpl_program *program = file->get_program();

	pstar_io io(r);

	ret = fill_input(r, io);
	if (ret != OK) {
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	ret = program->run(&io);
	if (ret != 0) {
		ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
				"P* Non-zero return (%i) from script %s", ret, r->filename);
		return HTTP_INTERNAL_SERVER_ERROR;
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

	try {
		try {
		 	return handle_file(r, filename, r->finfo.mtime);
		}
		catch (const runtime_error &e) {
			ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
					"P* Run error: %s: %s", e.what(), r->filename);
			return HTTP_INTERNAL_SERVER_ERROR;
		}
	}
	catch (const wpl_parser_exception &e) {
		ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
				"P* Parse error: %s: %s", e.what(), r->filename);
		return HTTP_INTERNAL_SERVER_ERROR;
	}
}
