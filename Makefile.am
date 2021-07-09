AUTOMAKE_OPTIONS = subdir-objects

SUBDIRS = pluto

bin_PROGRAMS = atiling

ATILING_INCLUDES = -I. -I$(top_builddir)/include -I$(top_srcdir)/include

ACLOCAL_AMFLAGS = -I m4

################################################################################

AM_CFLAGS = $(CFLAGS_WARN)
AM_CPPFLAGS = $(ATILING_INCLUDES) @OSL_CFLAGS@ @CLAN_CFLAGS@
AM_YFLAGS = -y -d

BUILT_SOURCES = src/parser.h


################################################################################

atiling_CPPFLAGS = $(ATILING_INCLUDES) @OSL_CFLAGS@ @CLAN_CFLAGS@
atiling_SOURCES = \
        src/parser.y \
        src/main.c \
        src/loop_info.c \
        src/scanner.l \
        src/atiling.c \
        src/options.c \
        src/fragment.c \
        src/gen.c \
        src/transform.c

atiling_LDADD = $(AC_LIBOBJ)



