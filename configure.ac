#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

#	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
#
#	Copyright (C) 2015-2020 Barcelona Supercomputing Center (BSC)

AC_PREREQ([2.69])
AC_INIT([tampi], [1.1], [pm-tools@bsc.es])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])

AC_LANG([C++])

# Prepare compilation flags before AC_PROG_CXX
AX_COMPILE_FLAGS

# Look for a valid MPI compiler
AC_PROG_CXX

# Retrieve compiler version
AX_CHECK_CXX_VERSION

# AM_PROG_AR must be called before LT_INIT or a warning ensues
AM_PROG_AR

# Automake initialization
AM_INIT_AUTOMAKE([foreign -Wall dist-bzip2 -Wno-portability subdir-objects silent-rules])
AM_SILENT_RULES([yes])

# Checks for header files.
AC_CHECK_HEADERS([unistd.h])

# Find and check the MPI library
LX_FIND_MPI
AX_CHECK_MPI

# Boost is no longer required
AX_BOOST_BASE([1.59], [], [ AC_MSG_ERROR([Boost library not found])])

# Check pthread
CHECK_PTHREAD

# Checks for typedefs, structures, and compiler characteristics.
AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Enable/disable blocking and non-blocking modes
AX_CHECK_TAMPI_MODES

AX_CHECK_CPUS

LT_INIT([shared static pic-only])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo ""
echo ""
echo "Configuration summary:"
echo "    Compiler version... ${CXX_VERSION}"
echo ""
echo "    Installation prefix... ${prefix}"
echo ""
echo "    TAMPI blocking mode... ${ac_blocking_mode}"
echo "    TAMPI non-blocking mode... ${ac_nonblocking_mode}"
echo ""
echo "    CXXFLAGS... ${tampi_CXXFLAGS} ${CXXFLAGS}"
echo "    CPPFLAGS... ${tampi_CPPFLAGS} ${CPPFLAGS}"
echo ""
echo "    MPI CXXFLAGS... ${MPI_CXXFLAGS}"
echo "    MPI LDFLAGS... ${MPI_CXXLDFLAGS}"
echo ""
echo "    Boost CPPFLAGS... ${BOOST_CPPFLAGS}"
echo "    Boost LDFLAGS... ${BOOST_LDFLAGS}"
