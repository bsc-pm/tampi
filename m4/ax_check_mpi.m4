#
# SYNOPSIS
#
#   AX_CHECK_MPI
#
# DESCRIPTION
#
#   Check whether MPI path to the headers and libraries are correctly specified.
#
# LICENSE
#
#   Copyright (c) 2015 Jorge Bellon <jbellon@bsc.es>
#
#   This program is free software: you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation, either version 3 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <http://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Archive. When you make and distribute a
#   modified version of the Autoconf Macro, you may extend this special
#   exception to the GPL to apply to your modified version as well.

AC_DEFUN([AX_CHECK_MPI],[
AC_PREREQ(2.59)dnl for _AC_LANG_PREFIX

#Check if an mpi implementation is installed.
AC_ARG_WITH(mpi,
[AS_HELP_STRING([--with-mpi,--with-mpi=PATH],
                [search in system directories or specify prefix directory for installed mpi package])])
AC_ARG_WITH(mpi-include,
[AS_HELP_STRING([--with-mpi-include=PATH],
                [specify directory for installed mpi include files])])
AC_ARG_WITH(mpi-lib,
[AS_HELP_STRING([--with-mpi-lib=PATH],
                [specify directory for the installed mpi library])])

# Search for mpi by default
if test "x$with_mpi" != xno; then
  mpi_prefix=$with_mpi
  mpiinc="-isystem $mpi_prefix/include"
  mpi_h="$mpi_prefix/include/mpi.h"
  AC_CHECK_FILE([$mpi_prefix/lib64],
    [mpilib=-L$mpi_prefix/lib64],
    [mpilib=-L$mpi_prefix/lib])
fi
if test "x$with_mpi_include" != x; then
  mpiinc="-isystem $with_mpi_include"
  mpi_h="$with_mpi_include/mpi.h"
fi
if test "x$with_mpi_lib" != x; then
  mpilib="-L$with_mpi_lib"
fi

# This is fulfilled even if $with_mpi="yes" 
# This happens when user leaves --with-value alone
if test x$with_mpi != xno; then

    #tests if provided headers and libraries are usable and correct
    bak_CFLAGS="$CFLAGS"
    bak_CPPFLAGS="$CPPFLAGS"
    bak_LIBS="$LIBS"
    bak_LDFLAGS="$LDFLAGS"

    CFLAGS=
    CPPFLAGS=$mpiinc
    LIBS=
    LDFLAGS=$mpilib

    # Check if mpi.h header file exists and compiles
    AC_CHECK_HEADER([mpi.h], [mpi=yes],[mpi=no])

    # Look for MPI_Init function in libmpi.so library
    if test x$mpi == xyes; then
      AC_CHECK_LIB([mpi],
                     [MPI_Init],
                     [mpi=yes
                      LIBS="$LIBS -lmpi"],
                     [mpi=no])
    fi

    mpilibs=$LIBS

    CFLAGS="$bak_CFLAGS"
    CPPFLAGS="$bak_CPPFLAGS"
    LIBS="$bak_LIBS"
    LDFLAGS="$bak_LDFLAGS"

    if test x$mpi != xyes; then
        AC_MSG_ERROR([
------------------------------
mpi path was not correctly specified. 
Please, check that the provided directories are correct.
------------------------------])
    fi
fi

AC_SUBST([mpi])
AC_SUBST([mpi_prefix])
AC_SUBST([mpiinc])
AC_SUBST([mpilib])
AC_SUBST([mpilibs])

])dnl AX_CHECK_MPI

