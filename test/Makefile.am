################################################################################
# Bench

PLUTO = $(realpath ../pluto/polycc)
ATILING = $(realpath ../atiling)

ADDCFLAGS=
CFLAGS = -DPOLYBENCH_TIME -O3 -I./polybench/utilities -I. -g 
CFLAGS+=$(ADDCFLAGS)
LDFLAGS = -lm -fopenmp

PLUTOFLAGS = --tile --parallel -q
ATILINGFLAGS = --transform

TEST_SRC = \
	polybench/linear-algebra/kernels/2mm/2mm.c \
	polybench/linear-algebra/kernels/3mm/3mm.c \
	polybench/datamining/correlation/correlation.c \
	polybench/datamining/covariance/covariance.c \
	polybench/linear-algebra/blas/syr2k/syr2k.c \
	polybench/linear-algebra/blas/symm/symm.c \
	polybench/linear-algebra/blas/gemm/gemm.c \
	polybench/linear-algebra/blas/trmm/trmm.c \
	benchmark/dsyr2k/dsyr2k.c \
	benchmark/ssymm/ssymm.c \
	benchmark/matmul/matmul.c


################################################################################

POLYBENCH_SRC = ./polybench/utilities/polybench.c
POLYBENCH_OBJ = ./polybench/utilities/polybench.o

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

TEST_OBJ = $(patsubst %.c, %.o, $(TEST_SRC)) \
		$(patsubst %.c, %_pluto_gen.o, $(TEST_SRC)) \
		$(patsubst %.c, %_atiling_gen.o, $(TEST_SRC))

TEST_SRC_ATILING = $(patsubst %.c, %_atiling.c, $(TEST_SRC))
TEST_OUT_ATILING = $(patsubst %.c, %_gen.c, $(TEST_SRC_ATILING))

TEST_SRC_PLUTO = $(patsubst %.c, %.c, $(TEST_SRC))
TEST_OUT_PLUTO = $(patsubst %.c, %_pluto_gen.c, $(TEST_SRC))

BASE_TARGETS = $(patsubst %.c, %, $(TEST_SRC))
PLUTO_TARGETS = $(addsuffix _pluto, $(BASE_TARGETS))
ATILING_TARGETS = $(addsuffix _atiling, $(BASE_TARGETS))

TARGETS = $(BASE_TARGETS) $(PLUTO_TARGETS) $(ATILING_TARGETS)

all: $(TARGETS)

$(TEST_OBJ): %.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

$(BASE_TARGETS): %: $(POLYBENCH_OBJ) %.o 
	$(CC) $^ -o $@ $(LDFLAGS)

$(PLUTO_TARGETS): %: $(POLYBENCH_OBJ) %_gen.o 
	$(CC) $^ -o $@ $(LDFLAGS) 

$(ATILING_TARGETS): %: $(POLYBENCH_OBJ) %_gen.o 
	$(CC) $^ -o $@ $(LDFLAGS)

%_atiling_gen.c: %_atiling.c
	$(ATILING) $(ATILINGFLAGS) -o $@ $^

%_pluto_gen.c: %.c
	(cd $(shell dirname $^) ; $(PLUTO) $(PLUTOFLAGS) -o $(shell basename $@) $(shell basename $^))

$(POLYBENCH_OBJ): $(POLYBENCH_SRC)
	$(CC) $(CFLAGS) -c $^ -o $@

clean-pluto:
	rm -f $(PLUTO_TARGETS)
	find . -type f -iname "*_pluto_gen*" -delete

clean-atiling:
	rm -f $(ATILING_TARGETS)
	find . -type f -iname "*_atiling_gen*" -delete

clean-targets:
	-rm -f $(TARGETS)
	find . -type f -iname "*.o" -delete

clean-local:
	-rm -rf $(TARGETS) */**.o ./*.cloog *.tmp
	find . -type f -iname "*.o" -delete
	find . -type f -iname "*_gen.c" -delete
	find . -type f -iname "*.cloog" -delete
