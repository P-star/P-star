#include <iostream>
#include <algorithm>

#ifdef WIN32
/*
#include "libmysql/libwpl_mysql.h"
MySQL disabled in Windows due to problems with linking
*/
#define PACKAGE_VERSION "1.1-alpha2"
#endif

#include "big_endian.h"
#include "libwpl/program.h"
#include "libwpl/module_loader.h"


using namespace std;

// Code for command line parsing
// from http://stackoverflow.com/questions/865668/parse-command-line-arguments
char *getCmdOption (char **begin, char **end, const std::string &option) {
	char ** itr = find(begin, end, option);
	if (itr != end && ++itr != end) {
		return *itr;
	}
	return 0;
}

bool cmdOptionExists(char **begin, char **end, const std::string &option) {
	return find(begin, end, option) != end;
}

void version() {
	cout << "The P* Web Programming Language interpreter version " PACKAGE_VERSION << "\n";
	cout << "Visit http://www.p-star.org/ for documentation\n\n";
}

void usage (char *argv0) {
	cout << argv0 << " [-h] -f filename\n";
	cout << "Licenced under GPLv3, Copyright MMXIII Atle Solbakken\n";
	cout << "Visit http://www.p-star.org/ for documentation\n\n";
}

#ifdef WIN32
int main(int argc, char *argv[], char *envp[]) {
#else
int main (int argc, char *argv[]) {
#endif
	if (cmdOptionExists(argv, argv+argc, "-h")) {
		usage (argv[0]);
		return 0;
	}
	if (cmdOptionExists(argv, argv+argc, "-v") || cmdOptionExists(argv, argv+argc, "--version")) {
		version ();
		return 0;
	}

	char *filename = getCmdOption (argv, argv+argc, "-f");
	if (filename == NULL) {
		usage(argv[0]);
		return -1;
	}

	int ret = 1;

	try {
#ifdef WIN32
		wpl_program program(argc, argv);
#else
		wpl_module_loader mysql_loader(argc, argv, "mysql");
		wpl_program program(argc, argv);
		program.set_parent_namespace(mysql_loader.get_namespace());
#endif

		program.parse_file(filename);

		ret = program.run();
	}
	catch (exception &e) {
		cout << e.what();
		return 1;
	}

	return ret;
}

