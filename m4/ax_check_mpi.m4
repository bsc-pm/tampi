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
  # 1) Save the previous value of autoconf generated variables.
  # They may be used later in the configuration (e.g. AC_OUTPUT)
  # 2) Save some important cached values.
  # This macro performs some checks using a different C++ compiler (mpicxx)
  # This is not very standar-ish, but allows us to avoid autoconf to reuse
  # AC_PROG_CXX checks that were done previously.
  # Of course, we will need to restore them before returning.
  # 3) Empty/set values
  # MPICH_IGNORE_CXX_SEEK and MPICH_SKIP_MPICXX are used to avoid
  # errors when mpi.h is included after stdio.h when compiling C++ code
  # It only applies, however, to mpich implementations
  AX_VAR_PUSHVALUE([CPPFLAGS])
  AX_VAR_PUSHVALUE([CXXFLAGS])
  AX_VAR_PUSHVALUE([LDFLAGS])
  AX_VAR_PUSHVALUE([LIBS])

  AC_ARG_VAR(MPICXX, [The MPI C++ compiler])

  AX_APPEND_FLAG([${MPI_CXXFLAGS}], [CXXFLAGS])
  AX_APPEND_FLAG([${MPI_CXXLDFLAGS}], [LDFLAGS])

  AC_DEFINE([MPICH_IGNORE_CXX_SEEK],[],[Avoid SEEK_SET conflict in mpicxx.h])
  AC_DEFINE([MPICH_SKIP_MPICXX],[],[Avoid including MPICH C++ bindings])
  AC_DEFINE([OMPI_SKIP_MPICXX],[],[Avoid including OMPI C++ bindings])

  AC_LANG_PUSH([C++])

  # Check if mpi.h and mpicxx.h header files exists and compiles
  AC_CHECK_HEADERS([mpi.h], [mpi=yes],[mpi=no])

  dnl  NOTE: Intel MPI checks are disabled for simplicity.
  dnl  User must provide either -mt_mpi -link_mpi=opt_mt using
  dnl  LDFLAGS variable at configure time.
  dnl  ----------------------------------------------------------
  dnl  Check if the provided MPI implementation is Intel MPI
  dnl  Multithread support will be provided using -link_mpi=flag.
  dnl  Valid values for flag are: opt opt_mt dbg dbg_mt
  dnl  Values with suffix '_mt' have multithread support.
  dnl impi_flag=
  dnl AS_IF([test "$enable_debug_mode" = yes],[
  dnl   impi_flag="-link_mpi=dbg_mt"
  dnl ],[
  dnl   impi_flag="-link_mpi=opt_mt"
  dnl ])
  dnl
  dnl AX_CHECK_LINK_FLAG(["$impi_flag"],[
  dnl   AX_APPEND_FLAG([ $impi_flag],[CXXFLAGS])
  dnl ],[
  dnl   # Older Intel MPI versions use -mt_mpi flag, which is now deprecated.
  dnl   AX_CHECK_LINK_FLAG([-mt_mpi],[
  dnl     AX_APPEND_FLAG([ -mt_mpi],[CXXFLAGS])
  dnl   ])
  dnl ])dnl

  # Look for MPI_Init_thread function in libmpi_mt, libmpi or libmpich libraries
  AS_IF([test x$mpi == xyes],[
    AC_SEARCH_LIBS([MPI_Init_thread],
                   [mpi_mt mpich mpi],
                   [mpi=yes;break],
                   [mpi=no])dnl
  ])dnl

  # If one of the previous tests were not satisfied, exit with an error message.
  AS_IF([test x$mpi != xyes],[
      AC_MSG_ERROR([
------------------------------
MPI path was not correctly specified.
Please, check that provided directories are correct.
------------------------------])
  ])dnl

  AC_SUBST([mpiflags])

  AX_VAR_POPVALUE([CPPFLAGS])
  AX_VAR_POPVALUE([CXXFLAGS])
  AX_VAR_POPVALUE([LDFLAGS])
  AX_VAR_POPVALUE([LIBS])

  AC_LANG_POP([C++])

])dnl AX_CHECK_MPI

