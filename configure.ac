#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([P* Web Programming Language Interpreter], [1.3], [atle@goliathdns.no])
AC_CONFIG_SRCDIR([src/])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# SOVERSION-stuff is from libshishi

# Library code modified:                              REVISION++
# Interfaces changed/added/removed:   CURRENT++       REVISION=0
# Interfaces added:                             AGE++
# Interfaces removed:                           AGE=0
AC_SUBST(LT_CURRENT, 2)
AC_SUBST(LT_REVISION, 3)
AC_SUBST(LT_AGE, 0)

# Used when creating libshishi-XX.def.
SOVERSION=`expr ${LT_CURRENT} - ${LT_AGE}`
AC_SUBST(SOVERSION)

AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE()
LT_INIT

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_CC_C_O
#AC_PATH_PROG(JAVA, java)

AC_ARG_WITH(mysql-dir,
        AC_HELP_STRING(
                [--with-mysql-dir],
                [Path to mysql_config [[/usr/bin]]]),
        [mysql_config_dir=$withval])

# This is stolen partially from Dovecot
AC_CHECK_PROG(MYSQL_CONFIG, mysql_config, mysql_config, NO, $PATH$PATH_SEPARATOR$withval)
if test $MYSQL_CONFIG = NO; then
	AC_MSG_ERROR([Could not find mysql_config. Specify with --with-mysql-dir=<path>])
else
	MYSQL_INCLUDES="`$MYSQL_CONFIG --include`"
	MYSQL_LIBS="`$MYSQL_CONFIG --libs`"
fi
AC_SUBST([MYSQL_INCLUDES])
AC_SUBST([MYSQL_LIBS])

# Stolen from http://stackoverflow.com/questions/3088069/boost-and-autoconf
AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([boost/foreach.hpp], [],
		[AC_MSG_ERROR(You need the Boost libraries.)])
AC_CHECK_HEADERS([mimetic/mimetic.h], [],
		[AC_MSG_ERROR(You need the Mimetic libraries.)])
AC_LANG_POP([C++])

AC_CHECK_LIB([dl], [dlopen], LIB_DL="-ldl")
AC_SUBST([LIB_DL])

#BOOST_REQUIRE([1.46.0])
#BOOST_REGEX

AC_CHECK_HEADERS([boost/regex.hpp],, [AC_MSG_ERROR([Boost.Regex (Boost C++ Libraries) is not installed, but is required by pstar])])

# Stolen from https://mod.gnutls.org/browser/mod_gnutls/configure.ac
AP_VERSION=2.2.22
CHECK_APACHE(,$AP_VERSION,
	:,:,
	AC_MSG_ERROR([*** Apache version $AP_VERSION not found!])
)

AP_MODULE_CFLAGS="${LIBGNUTLS_CFLAGS} ${SRP_CFLAGS} ${APR_MEMCACHE_CFLAGS} ${APXS_CFLAGS} ${AP_INCLUDES} ${APR_INCLUDES} ${APU_INCLUDES}"
AP_MODULE_LIBS="${APR_MEMCACHE_LIBS} ${LIBGNUTLS_LIBS}"

AC_SUBST([AP_MODULE_CFLAGS])
AC_SUBST([AP_MODULE_LIBS])

AC_DEFINE_UNQUOTED([WPL_BLOCK_MAX_CHILDREN], 128, [Maximum number of block elements])
AC_DEFINE_UNQUOTED([WPL_VARNAME_SIZE], 64, [Max variable name length])
AC_DEFINE_UNQUOTED([WPL_EXPRESSION_MAX], 64, [Maximum number of elements in an expression])
AC_DEFINE_UNQUOTED([WPL_MYSQL_BIND_MAX], 32, [Maximum number of variables which can be used in MySQL prepared statements])

AC_CONFIG_FILES([Makefile src/Makefile bin/Makefile src/libwpl/Makefile src/libmysql/Makefile src/apache2/Makefile])

#AC_DEFINE_UNQUOTED([WPL_DEBUG_EXPRESSIONS], 1, [Debug expression handling])
#AC_DEFINE_UNQUOTED([WPL_DEBUG_PARSING], 1, [Debug parsing])
#AC_DEFINE_UNQUOTED([WPL_DEBUG_FUNCTIONS], 1, [Debug function calls and argument handling])
#AC_DEFINE_UNQUOTED([WPL_DEBUG_TYPES], 1, [Debug types and casts])
#AC_DEFINE_UNQUOTED([WPL_DEBUG_DESTRUCTION], 1, [Debug object destruction])
#AC_DEFINE_UNQUOTED([WPL_DEBUG_BLOCKS], 1, [Debug blocks])
#AC_DEFINE_UNQUOTED([WPL_DEBUG_NAMESPACE], 1, [Debug namespace])

# Checks for libraries.
#PKG_CHECK_MODULES([GTKGL], [gtkglext-1.0])
#PKG_CHECK_MODULES([LIBXML], [libxml-2.0])

# Check for java directories
#AC_ARG_WITH(java-dir,
#        AC_HELP_STRING(
#                [--with-java-dir],
#                [Java class directory [[/usr/share/java]]]),
#        [classdir=$withval])


# Detect java package path
#AC_MSG_CHECKING([for java package svgSalamander.jar path])
#classdir_new="x"
#for try in $classdir /usr/share/java /usr/local/share/java /usr/local/java /opt/ext/java; do
#        if test -d "$try"; then
#		if test -f "$try/svgSalamander.jar"; then
#	                classdir_new="$try"
#        	        break
#		fi
#        fi
#done
#if test "$classdir_new" = "x"; then
#        AC_MSG_RESULT(no)
#        AC_MSG_ERROR([Could not find java package svgSalamander.jar. Specify with --with-java-dir=<path>])
#else
#        AC_MSG_RESULT([$classdir_new])
#fi
#classdir=$classdir_new:$classdir_new/svgSalamander.jar
#CLASSDIR="$classdir"
#AC_SUBST([CLASSDIR])
#AC_DEFINE_UNQUOTED([CLASSDIR], "$classdir", [Path for java classes])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

# For config.h
#if test "x$prefix" = "xNONE" ; then
#	prefix="/usr/local"
#fi

#DATADIR=`eval echo $datadir`
#DATADIR=`eval echo $DATADIR`
#AC_SUBST([DATADIR])
#AC_DEFINE_UNQUOTED([DATADIR], "$DATADIR/$PACKAGE", [Path for data files])

AC_OUTPUT()
