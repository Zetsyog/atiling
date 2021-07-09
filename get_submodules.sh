#!/bin/sh
git submodule init
git submodule update
(cd pluto; git submodule init; git submodule update)