#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([MPI-OmpSs interoperability library], [0.1], [jbellon@bsc.es])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])

AC_LANG([C++])
# MPICXX and MPICC variables will be read for compile checks
AC_ARG_VAR(MPICXX,[MPI C++ compiler command])
AC_ARG_VAR(MPICC,[MPI C compiler command])

# Check if debug mode is enabled
AX_DEBUG_MODE

# Look for a valid MPI compiler
AC_PROG_CXX([$CXX $MPICXX mpiicpc mpicxx])
AC_PROG_CC([$CC $MPICC mpiicc mpicc])

# AM_PROG_AR must be called before LT_INIT or a warning ensues
AM_PROG_AR

# Automake initialization
AM_INIT_AUTOMAKE([-Wall dist-bzip2 -Wno-portability subdir-objects])

# Checks for header files.
AC_CHECK_HEADERS([unistd.h])

# Check for libraries
# MPI
AX_CHECK_MPI

# OmpSs
AX_CHECK_OMPSS

# Argobots
AX_CHECK_ARGOBOTS

# EXTRAE
AX_CHECK_EXTRAE

# Checks for typedefs, structures, and compiler characteristics.
AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Turn off shared libraries.
LT_INIT([shared static])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo "- MPI-OmpSs interoperability library -"
echo "       - Configuration summary -"
echo "Installation prefix: $prefix"
