libmod_pstar_la_SOURCES = mod_pstar.cpp pstar_pool.cpp pstar_io.cpp
libmod_pstar_la_CPPFLAGS = -I$(top_srcdir)/src/libwpl -I$(top_srcdir)/include -include $(top_srcdir)/config.h
libmod_pstar_la_CXXFLAGS = -Wall ${AP_MODULE_CFLAGS} -std=c++11
libmod_pstar_la_LDFLAGS = -module -avoid-version ${AP_MODULE_LIBS} -L../libwpl
libmod_pstar_la_LIBADD = ../libwpl/libpstar.la
libmod_pstar_la_DEPENDENCIES = ../libwpl/libpstar.la

lib_LTLIBRARIES = libmod_pstar.la

clean:
	rm -f mod_pstar.so
	rm -f *.o *.lo *.la
	rm -fr .libs

install: ${lib_LTLIBRARIES}
	./install_mod.sh ${APXS_BIN}
