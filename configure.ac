                                                            
# Version 0.1.0
m4_define([version_major], [0])
m4_define([version_minor], [1])
m4_define([version_revision], [0])

# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([atiling], [version_major.version_minor.version_revision], [BUG-REPORT-ADDRESS])

AC_LANG(C)

AC_CONFIG_SRCDIR([include/atiling/atiling.h])

dnl --- Put as most as possible configuration files to an auxiliary dir
AC_CONFIG_AUX_DIR(autoconf)

dnl ############################################################################
dnl #                          Init automake                                   #
dnl ############################################################################
AM_INIT_AUTOMAKE([foreign subdir-objects])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

dnl ############################################################################
dnl #                            Checking                                      #
dnl ############################################################################

dnl # Checks for program
AC_PROG_CC
AC_PROG_LEX([noyywrap])
AC_PROG_YACC

dnl # Checks for header files.
AC_CHECK_HEADERS([ \
  inttypes.h \
  libintl.h \
  limits.h \
  malloc.h \
  stddef.h \
  stdint.h \
  stdlib.h \
  string.h \
  unistd.h
])


dnl # Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

dnl # Checks for library functions.
AC_CHECK_FUNCS([memset strdup])

dnl ############################################################################
dnl #                          OpenScop Library                                #
dnl ############################################################################
AC_SUBST(OSL_CFLAGS)
AC_SUBST(OSL_LIBS)
AC_SUBST(OSL_LDFLAGS)

dnl # Check if a custom path for osl is provided 
dnl # (else default path is /usr/local/osl)
AC_ARG_WITH([osl_prefix],
  [AS_HELP_STRING([--with-osl-prefix],
     [Prefix of osl installation (default is /usr/local/lib/osl)])],
  [osl_prefix=$withval],
  [osl_prefix=/usr/local/])

OSL_CFLAGS="-I$osl_prefix/include"
OSL_LDFLAGS="-L$osl_prefix/lib"
OSL_LIBS="-losl"

dnl # Check the headers (without polluting CPPFLAGS)
save_CPPFLAGS=$CFLAGS
CPPFLAGS="$CPPFLAGS $OSL_CFLAGS"
AC_CHECK_HEADER(osl/osl.h, [], [AC_MSG_ERROR("Can't find osl headers.")])
CPPFLAGS=$save_CPPFLAGS

dnl # Check the library (without polluting LDFLAGS)
save_LDFLAGS=$LDFLAGS
LDFLAGS="$LDFLAGS $OSL_LDFLAGS"
AC_CHECK_LIB(osl, osl_scop_malloc, [], [AC_MSG_ERROR("Can't find osl library.")])
LDFLAGS=$save_LDFLAGS

dnl ############################################################################
dnl #                            Clan Library                                  #
dnl ############################################################################
AC_SUBST(CLAN_CFLAGS)
AC_SUBST(CLAN_LIBS)
AC_SUBST(CLAN_LDFLAGS)

dnl # Check if a custom path for osl is provided 
dnl # (else default path is /usr/local/osl)
AC_ARG_WITH([clan_prefix],
  [AS_HELP_STRING([--with-clan-prefix],
     [Prefix of clan installation (default is /usr/local/)])],
  [clan_prefix=$withval],
  [clan_prefix=/usr/local/])

CLAN_CFLAGS="-I$clan_prefix/include"
CLAN_LDFLAGS="-L$clan_prefix/lib"
CLAN_LIBS="-lclan"

dnl # Check the headers (without polluting CPPFLAGS)
save_CPPFLAGS=$CFLAGS
CPPFLAGS="$CPPFLAGS $CLAN_CFLAGS"
AC_CHECK_HEADER(clan/clan.h, [], [AC_MSG_ERROR("Can't find osl headers.")])
CPPFLAGS=$save_CPPFLAGS

dnl # Check the library (without polluting LDFLAGS)
save_LDFLAGS=$LDFLAGS
LDFLAGS="$LDFLAGS $CLAN_LDFLAGS"
AC_CHECK_LIB(clan, clan_symbol_malloc, [], [AC_MSG_ERROR("Can't find osl library.")])
LDFLAGS=$save_LDFLAGS

dnl ############################################################################
dnl #                            Substitutions                                 #
dnl ############################################################################

AC_SUBST(VERSION_MAJOR)
AC_SUBST(VERSION_MINOR)
AC_SUBST(VERSION_REVISION)
AC_SUBST([YACC_PREFIX], [at_])

dnl # Config Makefile
AC_CONFIG_FILES([\
  Makefile \
  include/atiling/macros.h \
  include/atiling/yacc.h
])

AC_OUTPUT

