#include "io.h"

void wpl_io_standard::http_header(const char *field, const char *str) {
	cout << field << ": " << str << ";\r\n\r\n";
}
