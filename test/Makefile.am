################################################################################
# Tests

PLUTO = ../pluto/polycc
ATILING = ../atiling

CFLAGS = -DPOLYBENCH_TIME -O3
LDFLAGS = -lm -fopenmp

bin_PROGRAMS = 2mm 2mm_pluto 2mm_atiling

POLYBENCH_SRC = bench/polybench.c

2mm_SOURCES = \
	$(POLYBENCH_SRC) \
	bench/2mm.c

2mm_pluto_SOURCES = \
	$(POLYBENCH_SRC) \
	bench/2mm-pluto-gen.c

2mm_atiling_SOURCES = \
	$(POLYBENCH_SRC) \
	bench/2mm-atiling-gen.c

bench/2mm-atiling-gen.c: bench/2mm-atiling.c
	$(ATILING) -o $@ $<

bench/2mm-pluto-gen.c: bench/2mm.c
	$(PLUTO) --parallel --tile -o $@ $<

clean-local:
	-rm -f bench/2mm-atiling-gen.c bench/2mm-pluto-gen.c

