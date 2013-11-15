AM_CPPFLAGS = -I$(top_srcdir)/include -I../libwpl/ -include $(top_srcdir)/config.h $(MYSQL_INCLUDES)
AM_CXXFLAGS = -std=c++11 -O3

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libwpl_mysql-1.0.pc

lib_LTLIBRARIES = libwpl_mysql-1.0.la
libwpl_mysql_1_0_la_LIBADD = $(MYSQL_LIBS)
libwpl_mysql_1_0_la_SOURCES = \
	libwpl_mysql.cpp \
	sql.cpp value_sql.cpp \
	mysql.cpp mysql_stmt.cpp mysql_row.cpp \
	mysql_res_holder.cpp
