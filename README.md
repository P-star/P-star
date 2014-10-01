P*
==

P* (P-star) is a web programming language with in language-support for
things like HTML templates and database handling.

Contribute!
-----------
**P* is currently under development and help is needed to make it even better!**

Things to-be-made:
*   Real Windows port (.msi-package, fix MySQL-build)
*   OS X port
*   Port to Ubuntu, Fedora
*   Charset compatibility
*   Testing (make som web sites!!!)

Contact information can be found at the bottom of this document.

Building and running P*
-----------------------

To build and run P*, you need the libraries and development packages of
mysql-client and mimetic.  P* uses C++11 features, so GCC version 4.7 is
required.  If you use Clang, version 3.2 or higher is needed.  You also
need autotools installed.  Download and build P* using your favourite shell:

    git clone https://github.com/atlesn/P-star.git
    cd P-star
    autoreconf -i
    ./configure
    make install

You should specify an alternative directory to install P* so that you can
easily remove it later.  This is done by adding `prefix=my_directory` when
running configure, like `./configure --prefix=/usr/test`.  The files will
then be placed in /usr/test/bin and /usr/test/lib.

To run P* when you have specified an alternative install path, you should
run this first:

    export LD_LIBRARY_PATH=/usr/test/lib
    export PATH=$PATH:/usr/test/bin

A P*-program is run like this:

    wpl -f my_program.pstar


Using the Apache module
-----------------------
This package provides a module for the Apache-series web server, version
2.2.22 or higher is required. To enable the module after installing P*,
run this as root:

    a2enmod pstar
    service apache2 restart

To tell Apache to run P* scripts, add this directive somewhere in the
configuration (usually in <Directory>, <Location> or <File> sections).

    AddHandler pstar-handler .pstar


Examples
--------

Examples are found in directory "tests".


Contacts
--------

*   For information about P*, visit the official page: http://www.p-star.org/.

*   If you need support, ask on users' mailing list:
    p-star-users@googlegroups.com.

*   Should you decide to participate, say hello on the development mailing
    list: p-star-dev@googlegroups.com
