#!/bin/sh
xsltproc  --output  /var/www/pstar/manual/docbook.html /usr/share/xml/docbook/stylesheet/docbook-xsl/xhtml/chunk.xsl source.xml
mkdir /var/www/pstar/manual/images/ 2>&1 > /dev/null
cp -f ../../logo/logo.png /var/www/pstar/manual/images/
