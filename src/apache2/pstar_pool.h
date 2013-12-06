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

#include "../libwpl/program.h"
#include "../libwpl/exception.h"

#include "httpd.h"
#include "apr_proc_mutex.h"

#include <string>
#include <map>

struct pstar_file {
	private:
	int mtime;
	wpl_program program;

	public:
	bool is_modified (int mtime) {
		return (this->mtime != mtime ? true : false);
	}
	pstar_file (const char *filename, int mtime);
	wpl_program *get_program() {
		return &program;
	}
};

typedef map<string, shared_ptr<pstar_file>> pstar_map_t;

class pstar_pool {
	private:	
	pstar_map_t files;
	apr_proc_mutex_t *mutex;

	shared_ptr<pstar_file> get_file_handle(request_rec *r, const char *filename, int mtime);
	int handle_file (request_rec *r, const char *filename, int mtime);

	public:
	pstar_pool(apr_pool_t *pool);
	apr_status_t handle_request(request_rec *r);
};
