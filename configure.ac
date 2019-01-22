#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Task-Aware MPI Library], [0.10], [pm-tools@bsc.es])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])

AC_LANG([C++])
# MPICXX and implementation specific variables will be read for compile checks
AC_ARG_VAR(MPICXX,[MPI C++ frontend compiler])
AC_ARG_VAR(OMPI_CXX, [OpenMPI C++ backend compiler])
AC_ARG_VAR(MPICH_CXX,[MPICH C++ backend compiler])
AC_ARG_VAR(I_MPI_CXX,[Intel MPI C++ backend compiler])

AM_CONDITIONAL(DEFINES_OMPI_CXX, test x"${OMPI_CXX}" != x"")
AM_CONDITIONAL(DEFINES_MPICH_CXX, test x"${MPICH_CXX}" != x"")
AM_CONDITIONAL(DEFINES_I_MPI_CXX, test x"${I_MPI_CXX}" != x"")

# Check if debug mode is enabled
AX_DEBUG_MODE

# AC_PROG_CXX does not search for full paths,
# to allow MPICXX to be paths we need to set CXX
: ${CXX="$MPICXX"}

# Look for a valid MPI compiler
AC_PROG_CXX([$CXX $MPICXX mpiicpc mpicxx])

# AM_PROG_AR must be called before LT_INIT or a warning ensues
AM_PROG_AR

# Automake initialization
AM_INIT_AUTOMAKE([foreign -Wall dist-bzip2 -Wno-portability subdir-objects silent-rules])
AM_SILENT_RULES([yes])

# Checks for header files.
AC_CHECK_HEADERS([unistd.h])

# Check for libraries
# MPI
LX_FIND_MPI
AX_APPEND_FLAG([${MPI_CXXFLAGS}], [CXXFLAGS])
AX_APPEND_FLAG([${MPI_CXXLDFLAGS}], [LDFLAGS])
AX_CHECK_MPI

# Boost is no longer required
#AX_BOOST_BASE([1.59], [], [ AC_MSG_ERROR([Boost library not found])])

# Checks for typedefs, structures, and compiler characteristics.
AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Enable/disable blocking and non-blocking modes
AX_CHECK_BLOCKING_MODE
AX_CHECK_NONBLOCKING_MODE
AX_NOTIFY_EXTERNAL_EVENTS_API

# Turn off shared libraries.
LT_INIT([static])

# Enable/disable architecture specific code
AS_CASE([$host],
  [x86_64*],[ARCH=x86_64])
AS_IF([test "$ARCH" = "x86_64"],[
  AC_DEFINE([X86_64_ARCH],[1],[Compiling for x86_64 host])
])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo ""
echo ""
echo "Configuration summary:"
echo "    Installation prefix... ${prefix}"
echo ""
echo "    MPI CXX compiler... ${MPICXX}"
# echo ""
# echo "    Boost CPPFLAGS... ${BOOST_CPPFLAGS}"
# echo "    Boost LDFLAGS... ${BOOST_LDFLAGS}"
