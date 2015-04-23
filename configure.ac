#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([MPI-OmpSs interoperability library], [0.1], [jbellon@bsc.es])

AC_LANG([C++])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])

# AM_PROG_AR must be called before LT_INIT or a warning ensues
AM_PROG_AR

# Turn off static libraries.
LT_INIT([static disable-shared])

# Automake initialization
AM_INIT_AUTOMAKE([-Wall dist-bzip2 -Wno-portability subdir-objects])

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.
# OmpSs
AX_CHECK_OMPSS

# MPI
AX_CHECK_MPI

# Checks for header files.
AC_CHECK_HEADERS([unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo "- MPI-OmpSs interoperability library -"
echo "       - Configuration summary -"
echo "Installation prefix: $prefix"
