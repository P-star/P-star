#!/bin/bash

cp -rfv manual/ /var/www/pstar/
cp -fv .htaccess * /var/www/pstar/
chmod -R 644 /var/www/pstar/*
chmod 755 /var/www/pstar/

