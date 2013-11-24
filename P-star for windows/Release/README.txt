P* Web Programming Language
===========================

P* (P-star) is a web programming language whith in language-support for things like HTML templates and database handling.

P* is free software licenced under GPLv3.

P* is currently under development and help is needed!

Please visit the web site for tutorials, examples and documentation.

    http://www.p-star.org


-- DISCLAIMER
This is the official Windows-build of the P* interpreter. It is built and tested on Windows 7, but there is no guarantee that it will run on all Windows installs. Please report any problems using the issue-tracking system (see "BUGS & ISSUES".


-- HOW TO RUN?
You have obtained a precompiled ready to run Windows-port of P* with these four binaries:

    libboost_regex-vc120-mt-1_55.lib
    libboost_regex-vc120-mt-gd-1_55.lib
    libmimetic_d.lib
    wpl.exe

1. Copy all four files into a directory, for instance "C:\P-star".

2. Open a command line window

3. Add the new directory into the PATH variable by running this:

    set PATH=%PATH%;C:\\P-star

4. Run P* scripts like this:

    wpl -f filename.pstar


-- BUGS & ISSUES

Please report any bugs or missing features on Github:

    https://github.com/atlesn/P-star/issues


-- CONTACT

Developer Atle Solbakken

    atle@goliathdns.no
