#!/bin/sh
git submodule init
git submodule update
(cd pluto; git submodule init; git submodule update; ./apply_patches.sh)

mkdir trahrhe
rm -rf trahrhe/*
cd trahrhe
wget https://gforge.inria.fr/frs/download.php/file/38349/trahrhe-1.0.zip
unzip -q trahrhe-1.0.zip && rm trahrhe-1.0.zip
