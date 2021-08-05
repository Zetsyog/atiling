#!/bin/sh
git submodule init
git submodule update
(cd pluto; git submodule init; git submodule update; ./apply_patches.sh)

rm -rf trahrhe/
mkdir trahrhe
cd trahrhe
wget https://gforge.inria.fr/frs/download.php/file/38349/trahrhe-1.0.zip -O trahrhe.zip
unzip -q trahrhe.zip && rm trahrhe.zip

