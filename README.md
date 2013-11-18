P*
==

P* (P-star) is a web programming language whith in language-support for things like HTML-templates and Database handling..

P* is currently under development, and help is needed!

Please visit this page for information about P*: 
  http://www.p-star.org/

Ask on this mailing list if you need support!
p-star-users@googlegroups.com

Say hello on the mailing list to participate!
p-star-dev@googlegroups.com

Take a look in the tests/-directory for some examples.

To build and run P*, you need to have the libraries and development packages of mysql-client and mimetic. You also need autotools installed. This is how you download and build P* using your favourite shell:

git clone https://github.com/atlesn/P-star.git
cd P-star
autoreconf -i
./configure
make install
			

You should specify an alternative directory to install P* in so that you easilly can remove it later. This is done by adding prefix=my_directory when running configure, like ./configure --prefix=/usr/test. The files will then be placed in /usr/test/bin and /usr/test/lib.

To run P* when you have specified an alternative install path, you should run this first:

export LD_LIBRARY_PATH=/usr/test/lib
export PATH=$PATH:/usr/test/bin
			
A P*-program is run like this:

wpl -f my_program.pstar


