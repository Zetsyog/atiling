#!/bin/bash

echo -e "Starting tests"

printf "Base : "
./test/2mm
printf "Pluto : "
./test/2mm_pluto
printf "Atiling : "
./test/2mm_atiling

echo -e "Done."
