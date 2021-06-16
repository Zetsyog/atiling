AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = atiling

ATILING_INCLUDES = -I. -I$(top_builddir)/include -I$(top_srcdir)/include

################################################################################

AM_CFLAGS = $(CFLAGS_WARN)
AM_CPPFLAGS = $(ATILING_INCLUDES) @OSL_CFLAGS@ @CLAN_CFLAGS@
AM_YFLAGS = -yd

BUILT_SOURCES = src/parser.h


################################################################################

atiling_CPPFLAGS = $(ATILING_INCLUDES) @OSL_CFLAGS@ @CLAN_CFLAGS@
atiling_SOURCES = \
        src/main.c \
        src/loop_info.c \
        src/parser.y \
        src/scanner.l \
        src/atiling.c \
        src/options.c \
        src/pragma.c \
        src/gen.c

atiling_LDADD = $(LIBOBJS)



