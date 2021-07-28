################################################################################

ATILING_INCLUDES = -I. -I$(top_builddir)/include -I$(top_srcdir)/include -I$(top_srcdir)/src

AM_CFLAGS = $(CFLAGS_WARN)
AM_CPPFLAGS = $(ATILING_INCLUDES) -g
AM_YFLAGS = -y -d

BUILT_SOURCES = parser.h

################################################################################

bin_PROGRAMS = atiling

atiling_SOURCES = \
	parser.y \
	main.c \
	loop_info.c \
	scanner.l \
	atiling.c \
	options.c \
	fragment.c \
	gen.c \
	transform.c

atiling_CXXFLAGS = $(OPT_FLAGS) $(DEBUG_FLAGS) -O0 \
	-DSCOPLIB_INT_T_IS_LONGLONG -DCLOOG_INT_GMP -DPLUTO_OPENCL \
	$(ATILING_INCLUDES) \
	-I$(top_srcdir)/pluto/openscop/include \
	-I$(top_srcdir)/pluto/clan/include \
	-I$(top_srcdir)/pluto/isl/include -I$(top_builddir)/pluto/isl/include \
	-I$(top_srcdir)/pluto/include \
	-I$(top_srcdir)/pluto/tool \
	-I$(top_srcdir)/pluto/lib

atiling_CFLAGS = $(AM_CFLAGS) $(atiling_CXXFLAGS)

atiling_LDADD =  \
	$(AC_LIBOBJ) \
	../pluto/openscop/libosl.la \
	../pluto/clan/libclan.la \
	../pluto/lib/libpluto.la \
	../pluto/tool/libpluto_tool.la \
	-lm -lgomp
