#!/bin/sh
xsltproc  --output  /var/www/html/default/pstar/docbook.html /usr/share/xml/docbook/stylesheet/docbook-xsl/xhtml/chunk.xsl source.xml
mkdir /var/www/html/default/pstar/images/ 2>&1 > /dev/null
cp -f ../../logo/logo.png /var/www/html/default/pstar/images/
