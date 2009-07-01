#! /bin/sh

set -x
aclocal-1.7 && \
autoheader && \
automake --no-force --add-missing --copy --gnits && \
autoconf && \
./configure "$@" && \
make

