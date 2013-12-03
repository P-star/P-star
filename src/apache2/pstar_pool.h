#include "httpd.h"

#include "../libwpl/program.h"
#include "../libwpl/exception.h"

#include <string>
#include <map>

struct pstar_file {
	private:
	int mtime;
	wpl_program program;

	public:
	bool check_modified (int mtime) {
		return (this->mtime != mtime ? false : true);
	}
	pstar_file (const char *filename, int mtime);
};

class pstar_pool {
	private:	
	map<string, pstar_file> files;

	void handle_file (const char *filename, int mtime);

	public:
	apr_status_t handle_request(request_rec *r);
};
