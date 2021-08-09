PLUTO = ../../pluto/polycc
ATILING = ../../atiling

CFLAGS = -DPOLYBENCH_TIME -O3 -I./utilities -g
LDFLAGS = -lm -fopenmp

PLUTOFLAGS = --nounrolljam --tile --parallel --noprevector -q

POLYBENCH_SRC = ./utilities/polybench.c
POLYBENCH_OBJ = ./utilities/polybench.o

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

TEST_SRC = \
	linear-algebra/kernels/2mm/2mm.c

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
	$(ATILING) -o $@ $^

%_pluto_gen.c: %.c
	$(PLUTO) $(PLUTOFLAGS) -o $@ $^

$(POLYBENCH_OBJ): $(POLYBENCH_SRC)
	$(CC) $(CFLAGS) -c $^ -o $@

clean-local:
	-rm -rf $(TARGETS) */**.o
