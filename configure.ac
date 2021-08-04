                                                            
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
AC_CONFIG_MACRO_DIR([m4])

LT_INIT()

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
dnl #                                Pluto                                     #
dnl ############################################################################
echo ""
echo "==========================="
echo "Configuring Pluto"
echo "==========================="
AC_CONFIG_SUBDIRS([pluto])


dnl ############################################################################
dnl #                            Substitutions                                 #
dnl ############################################################################

AC_SUBST(VERSION_MAJOR)
AC_SUBST(VERSION_MINOR)
AC_SUBST(VERSION_REVISION)
AC_SUBST([YACC_PREFIX], [at_])

AC_PATH_PROGS(BASH, bash)

dnl # Config Makefile
AC_CONFIG_FILES([\
  include/atiling/macros.h \
  include/atiling/yacc.h
])

AC_OUTPUT([
  Makefile \
  src/Makefile \
  test/Makefile \
  atiling.sh
])

echo " /*-----------------------------------------------*"
echo "  *          ATILING configuration is OK          *"
echo "  *-----------------------------------------------*/"
echo "Your system is ready to compile ATILING"
echo "Run 'make' next to compile"
# echo "Then, run 'make test' to check if everything is working correctly"
echo "Use atiling (see README)"
