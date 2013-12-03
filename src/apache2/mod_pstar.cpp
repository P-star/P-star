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
	pool_global = new pstar_pool();
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
