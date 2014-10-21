/*
   This file is part of P-star (http://www.p-star.org), and
   is licenced to you under the Apache Licence, Version 2.0.
   You should have received a copy of the licence with this
   software, but it can also be obtained here:

      http://www.apache.org/licenses/LICENSE-2.0
   
   - Copyright MMXIII Atle Solbakken
      http://www.p-star.org
      atle@goliathdns.no 
   
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "pstar_io.h"

#include "http_log.h"
#include "http_protocol.h"
#include "util_script.h"
#include "apr_lib.h"
#include "apr_strings.h"

#include "apache_24.h"

pstar_io::pstar_io(request_rec *r) : wpl_io() {
	this->r = r;
	this->rpos = 0;
	this->headers_sent = false;
	this->waiting_buckets = 0;

	this->ba = apr_bucket_alloc_create(r->pool);
	this->bb = apr_brigade_create(r->pool, this->ba);
}

pstar_io::~pstar_io() {
	apr_status_t rv;
	apr_bucket *b;

	static const char msg[] = "<h1>P* Error</h1><p>An error occured. More information can be found in the web server log files.</p>";

	if (http_error_pending) {
		apr_table_set (r->headers_out, "Content-Type", "text/html");

//		apr_brigade_cleanup(bb);
		b = apr_bucket_immortal_create(msg, sizeof(msg), ba);
		APR_BRIGADE_INSERT_TAIL(bb, b);
	}

	b = apr_bucket_eos_create(ba);
	APR_BRIGADE_INSERT_TAIL(bb, b);

	rv = ap_pass_brigade(r->output_filters, bb);
	if (rv != APR_SUCCESS) {
		throw runtime_error("pstar_io::write(); Could not write to client");
	}
}

void pstar_io::http_header(const char *field, const char *str) {
	if (headers_sent) {
		throw runtime_error("pstar_io::http_header(): Headers have already been sent");
	}
	/*
	   TODO
	   Replace with emplace() when Debian stops using GCC 4.7
	 */
	http_headers.insert(std::pair<string,string>(field, str));
}

void pstar_io::append_input(const char *str, int len) {
	input.append(str, len);
}

void pstar_io::read (char *str, int len) {
	const char *pos = input.c_str() + rpos;
	const char *max = input.c_str() + input.size();
	int remain = max - pos;

	if (len > remain) {
		len = remain;
	}

	if (len == 0) {
		str[0] = '\0';
		return;
	}

	strncpy (str, pos, len);
	str[len] = '\0';

	rpos += len;
}

void pstar_io::write(const char *str, int len) {
	write_immortal(apr_pstrmemdup(r->pool, str, len), len);
/*	if (!headers_sent) {
		output_headers();
	}
	apr_status_t rv;
	apr_bucket *b;

	ostringstream msg;
	msg << "BT " << len << " - " << str;
	debug(msg.str().c_str());

	b = apr_bucket_transient_create (str, len, ba);
	APR_BRIGADE_INSERT_TAIL(bb, b);*/
/*
	if (waiting_buckets++ % 100 == 0) {
		rv = ap_pass_brigade(r->output_filters, bb);
		if (rv != APR_SUCCESS) {
			throw runtime_error("pstar_io::write(); Could not write to client");
		}
		apr_brigade_cleanup(bb);
	}*/
}

void pstar_io::write_immortal(const char *str, int len) {
	if (!headers_sent) {
		output_headers();
	}
	apr_status_t rv;
	apr_bucket *b;

	b = apr_bucket_immortal_create (str, len, ba);
	APR_BRIGADE_INSERT_TAIL(bb, b);

	if (waiting_buckets++ % 100 == 0) {
		rv = ap_pass_brigade(r->output_filters, bb);
		if (rv != APR_SUCCESS) {
			throw runtime_error("pstar_io::write(); Could not write to client");
		}
		apr_brigade_cleanup(bb);
	}
}

void pstar_io::output_headers () {
	headers_sent = true;
	for (const pair<string,string> &my_pair : http_headers) {
		apr_table_set (r->headers_out, my_pair.first.c_str(), my_pair.second.c_str());
	}
}

const char *pstar_io::get_env(const char *name) {
	return apr_table_get (r->subprocess_env, name);
}

void pstar_io::debug(const char *str) {
	ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "%s", str);
}

void pstar_io::error(const char *str) {
	http_error_pending = true;
	ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "%s", str);
}

