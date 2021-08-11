#!/bin/sh
git submodule init
git submodule update
(cd pluto; git submodule init; git submodule update; ./apply_patches.sh)

rm -rf trahrhe/
mkdir trahrhe
cd trahrhe
wget https://webpages.gitlabpages.inria.fr/trahrhe/trahrhe_packages/trahrhe-2.0.zip -O trahrhe.zip
unzip -q trahrhe.zip && rm trahrhe.zip

