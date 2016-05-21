#!/bin/sh

./autoclean.sh

rm -f configure

rm -f Makefile.in

rm -f config.guess
rm -f config.sub
rm -f install-sh
rm -f missing
rm -f depcomp

if [ 0 = 1 ]; then
autoscan
else

touch NEWS
touch README
touch AUTHORS
touch ChangeLog
touch config.h.in

#libtoolize --copy --force
aclocal
automake -a -c
autoconf
autoreconf -f -i -Wall,no-obsolete
autoreconf -f -i -Wall,no-obsolete

#./configure --enable-debug
#./configure

#make clean
#make

fi
