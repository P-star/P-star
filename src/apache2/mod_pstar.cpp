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

#include "pstar_pool.h"

#include "apr_hash.h"
#include "ap_config.h"
#include "ap_provider.h"
#include "httpd.h"
#include "http_core.h"
#include "http_config.h"
#include "http_log.h"
#include "http_protocol.h"
#include "http_request.h"

#include <stdio.h>

extern "C" {

static pstar_pool *pool_global = NULL;

apr_status_t pstar_pool_destruct (void *arg) {
	pstar_pool *pool = (pstar_pool*) arg;
	delete pool;
	return APR_SUCCESS;
}

static void pstar_pool_init (apr_pool_t *pool) {
	pool_global = new pstar_pool(pool);
	apr_pool_cleanup_register (pool, pool_global, pstar_pool_destruct, NULL);
}

static int pstar_handler (request_rec *r) {
	if (!r->handler || strcmp(r->handler, "pstar-handler")) return (DECLINED);

	return pool_global->handle_request(r);
}

static void register_hooks (apr_pool_t *pool) {
	ap_hook_handler(pstar_handler, NULL, NULL, APR_HOOK_LAST);

	pstar_pool_init(pool);
}

module AP_MODULE_DECLARE_DATA pstar_module = {
	STANDARD20_MODULE_STUFF,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	register_hooks
};

}
