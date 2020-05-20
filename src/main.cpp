#include <iostream>
#include <algorithm>

#ifdef WIN32
/*
#include "libmysql/libwpl_mysql.h"
MySQL disabled in Windows due to problems with linking
*/
#define PACKAGE_VERSION "1.1-alpha2"
#endif

#include "libwpl/program.h"
#include "libwpl/io.h"

#include <cstdlib>

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
	cout << "The P* Web Programming Language interpreter version " PACKAGE_VERSION <<
		" build " __DATE__ " " __TIME__ "\n";
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
		filename = getenv("PSTAR_FILE");
		if (filename == NULL) {
			usage(argv[0]);
			return -1;
		}
	}

#ifdef WIN32
	const char *env_tmp = "PSTAR_ROOT=.";
	_putenv(env_tmp);
#else
	setenv("PSTAR_ROOT", ".", 1);
#endif

	int ret = 1;

	wpl_io_standard output;
	wpl_program program(output, argc, argv);

	try {
		try {
			program.parse_file(filename);
		}
		catch (wpl_parser_exception &e) {
			cerr << e.what() << endl;
			return 1;
		}

		ret = program.run(output);
	   }
	  catch (wpl_element_exception &e) {
		e.output(output);
		return 1;
	}

	return ret;
}

