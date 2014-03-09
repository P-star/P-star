#pragma once

// first undefine offending macros
// that are present in httpd.h
// and config.h
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION

#include "httpd.h"

// now we are using macros from httpd.h
// so undefine them again and
// include config.h in order to use
// 'our' version of macros
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION

#include "../config.h"
