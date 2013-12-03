libmod_pstar_la_SOURCES = mod_pstar.c
libmod_pstar_la_CFLAGS = -Wall ${AP_MODULE_CFLAGS}
libmod_pstar_la_LDFLAGS = -rpath ${AP_LIBEXECDIR} -module -avoid-version ${AP_MODULE_LIBS}
	
lib_LTLIBRARIES = libmod_pstar.la

make_so: $(lib_LTLIBRARIES)
	@if test ! -L mod_pstar.so ; then ln -s .libs/libmod_pstar.so mod_pstar.so ; fi

clean:
	rm -f mod_pstar.so
	rm -f *.o *.lo *.la
	rm -fr .libs

install: make_so
	./install_mod.sh ${APXS_BIN}
