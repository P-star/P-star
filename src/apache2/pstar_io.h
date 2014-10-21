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

#include "io.h"

#include "pstar_httpd.h"

#include <map>
#include <sstream>

class pstar_io : public wpl_io {
	private:
	string input;
	int rpos;
	int waiting_buckets;

	bool headers_sent = false;
	bool http_error_pending = false;

	map<string,string> http_headers;

	request_rec *r;
	apr_bucket_brigade *bb;
	apr_bucket_alloc_t *ba;
	
	public:
	pstar_io (request_rec *r);
	~pstar_io ();

	void read (char *str, int len) override;
	void write (const char *str, int len) override;
	void write_immortal (const char *str, int len) override;
	const char *get_env(const char *name) override;
	void http_header (const char *field, const char *str) override;
	void debug (const char *str) override;
	void error (const char *str) override;

	void append_input (const char *str, int len);
	void output_headers ();
};
