#!/bin/sh

echo -e "\n*** Running autotools on atiling ***"
autoreconf -vi

if test -f pluto/autogen.sh; then
    (cd pluto; ./autogen.sh)
fi

