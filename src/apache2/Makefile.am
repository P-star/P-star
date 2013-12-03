libmod_pstar_la_SOURCES = mod_pstar.cpp
libmod_pstar_la_CXXFLAGS = -Wall ${AP_MODULE_CFLAGS}
libmod_pstar_la_LDFLAGS = -rpath ${AP_LIBEXECDIR} -module -avoid-version ${AP_MODULE_LIBS}
	
lib_LTLIBRARIES = libmod_pstar.la

clean:
	rm -f mod_pstar.so
	rm -f *.o *.lo *.la
	rm -fr .libs

install: ${lib_LTLIBRARIES}
	./install_mod.sh ${APXS_BIN}
