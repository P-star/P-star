#!/bin/bash

APXS_BIN=$1

if [ "$APXS_DH_CONTEXT" == "1" ]; then
	cd ../..
	cp src/apache2/.libs/libmod_pstar.so debian/libapache2-mod-pstar/usr/lib/apache2/modules/mod_pstar.so
else
	$APXS_BIN -i -n mod_pstar .libs/libmod_pstar.so
fi

