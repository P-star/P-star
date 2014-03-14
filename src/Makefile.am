SUBDIRS = libwpl/ libmysql/ apache2/

AM_CPPFLAGS = -I$(top_srcdir)/include -include $(top_srcdir)/config.h
AM_CXXFLAGS = -std=c++11
bin_PROGRAMS = wpl

wpl_LDFLAGS = -Wl,--no-as-needed -ldl -rdynamic
wpl_LDADD = libwpl/libpstar.la
wpl_SOURCES = main.cpp

#extension_CPPFLAGS = $(GTKGL_CFLAGS) $(LIBXML_CFLAGS) -std=c++0x
#extension_CFLAGS = $(GTKGL_CFLAGS)
#extension_LDADD = $(GTKGL_LIBS) $(LIBXML_LIBS)

