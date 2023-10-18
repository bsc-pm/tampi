#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

#	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
#
#	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)

AC_PREREQ([2.69])
AC_INIT([tampi], [2.0], [pm-tools@bsc.es])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])

AC_LANG([C++])

# Prepare compilation flags before AC_PROG_CXX
AX_COMPILE_FLAGS

# Look for a valid C++ compiler
AC_PROG_CXX

# Retrieve compiler version
AX_CHECK_CXX_VERSION

# AM_PROG_AR must be called before LT_INIT or a warning ensues
AM_PROG_AR

# Automake initialization
AM_INIT_AUTOMAKE([foreign -Wall dist-bzip2 -Wno-portability subdir-objects silent-rules])
AM_SILENT_RULES([yes])

# Checks header files
AC_CHECK_HEADERS([unistd.h])

# Find and check the MPI library
LX_FIND_MPI
AX_CHECK_MPI

# Check the Boost library
AX_BOOST_BASE([1.59], [], [ AC_MSG_ERROR([Boost library not found])])

# Check the ovni instrumentation library
AC_CHECK_OVNI

# Check that the compiler supports C++17
AX_CXX_COMPILE_STDCXX_17([noext], [mandatory])

# Enable/disable blocking and non-blocking modes
AX_CHECK_TAMPI_MODES

# Check the CPU count of the system
AX_CHECK_CPUS

# Compile both static and dynamic libraries. All objects should be compiled
# enabling the position independent code (PIC) mode
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
echo "    CXXFLAGS... ${tampi_CXXFLAGS} ${asan_CXXFLAGS} ${CXXFLAGS}"
echo "    CPPFLAGS... ${tampi_CPPFLAGS} ${asan_CPPFLAGS} ${CPPFLAGS}"
echo "    LDFLAGS... ${asan_LDFLAGS} ${LDFLAGS}"
echo ""
echo "    MPI CXXFLAGS... ${MPI_CXXFLAGS}"
echo "    MPI LDFLAGS... ${MPI_CXXLDFLAGS}"
echo ""
echo "    Boost CPPFLAGS... ${BOOST_CPPFLAGS}"
echo ""
echo "    Ovni is enabled... ${ac_use_ovni}"

if test x"${ac_use_ovni}" = x"yes" ; then
	echo "    Ovni CPPFLAGS... ${ovni_CPPFLAGS}"
	echo "    Ovni LIBS... ${ovni_LIBS}"
fi
echo ""
