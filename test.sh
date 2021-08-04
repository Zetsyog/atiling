#!/bin/bash

echo -e "Starting tests"

printf "Base"
./test/bench/2mm
printf "Pluto"
./test/bench/2mm_pluto
printf "Atiling"
./test/bench/2mm_atiling

echo -e "Done."
