#include "pstar_pool.h"
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

void pstar_pool::handle_file (const char *filename, int mtime) {
	/*
	   TODO
	   Cache file parsing (problems with GCC4.7 map::emplace() )
	   */
	wpl_program file(0, NULL);
	file.parse_file(filename);
	int ret = file.run();
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
		 	handle_file(filename, r->finfo.mtime);
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

	return APR_SUCCESS;
}
