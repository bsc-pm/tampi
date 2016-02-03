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
  AX_VAR_PUSHVALUE([LDFLAGS])
  AX_VAR_PUSHVALUE([LIBS])

  AC_DEFINE([MPICH_IGNORE_CXX_SEEK],[],[Avoid SEEK_SET conflict in mpicxx.h])
  AC_DEFINE([MPICH_SKIP_MPICXX],[],[Avoids inclusion of C++ bindings. This library does not use them.])

  AC_LANG_PUSH([C++])

  # Check if mpi.h and mpicxx.h header files exists and compiles
  AC_CHECK_HEADERS([mpi.h], [mpi=yes],[mpi=no])

  # Check if the provided MPI implementation is Intel MPI
  # Multithread support will be provided using -link_mpi=flag.
  # Valid values for flag are: opt opt_mt dbg dbg_mt
  # Values with suffix '_mt' have multithread support.
  impi_flag=
  AS_IF([test "$enable_debug_mode" = yes],[
    impi_flag="-link_mpi=dbg_mt"
  ],[
    impi_flag="-link_mpi=opt_mt"
  ])

  AX_CHECK_COMPILE_FLAG(["$impi_flag"],[
    AX_APPEND_FLAG([ $impi_flag],[CXXFLAGS])
  ],[
    # Older Intel MPI versions use -mt_mpi flag, which is now deprecated.
    AX_CHECK_COMPILE_FLAG([-mt_mpi],[
      AX_APPEND_FLAG([ -mt_mpi],[CXXFLAGS])
    ])
  ])dnl

  # Look for MPI_Init_thread function in libmpi_mt, libmpi or libmpich libraries
  AS_IF([test x$mpi == xyes],[
    AC_SEARCH_LIBS([MPI_Init_thread],
                   [mpi_mt mpich mpi],
                   [mpi=yes;break],
                   [mpi=no])dnl
  ])dnl

  # Look for MPI::Comm::Comm() function in libmpicxx, libmpi_cxx or libmpichcxx libraries
  AS_IF([test x$mpi == xyes],[
    AC_SEARCH_LIBS([_ZN3MPI4CommD0Ev],
                   [mpichcxx mpi_cxx],
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

  # Check that the MPI library supports multithreading (MPI_THREAD_MULTIPLE)
  AS_IF([test x$mpi = xyes],[
    AC_CACHE_CHECK([MPI library multithreading support],[ac_cv_mpi_mt],
      [AC_RUN_IFELSE(
        [AC_LANG_PROGRAM(
          [
             #include <string>

             #include <stdio.h>
             #include <stdlib.h>
             #include <unistd.h>
             // Include mpi.h after stdio.h
             // Avoid MPICH 'SEEK_SET already defined' error
             #ifdef HAVE_MPI_H
               #include <mpi.h>
             #endif
          ],
          [
             // This library likes printing error messages through stdout
             // redirect them to stderr
             close(1);
             dup(2);

             /* Initialize MPI library. */
             int err = MPI_SUCCESS;
             int provided;
             err = MPI_Init_thread( NULL, NULL, MPI_THREAD_MULTIPLE, &provided );
             if (err != MPI_SUCCESS) {
                 fprintf(stderr, "MPI_Init_thread failed with error %d\n", err );
                 return 1;
             }

             std::string mt_support;
             switch( provided ) {
                 case MPI_THREAD_SINGLE:
                 mt_support = "none";
                 break;

                 case MPI_THREAD_FUNNELED:
                 mt_support = "funneled";
                 break;

                 case MPI_THREAD_SERIALIZED:
                 mt_support = "serialized";
                 break;

                 case MPI_THREAD_MULTIPLE:
                 mt_support = "concurrent";
                 break;

                 default:
                 mt_support = "(invalid value)";
                 break;
             }

             FILE* out = fopen("conftest.out","w");
             fprintf(out,"%s\n", mt_support.c_str());
             fclose(out);

             return 0;
          ])],
        [ac_cv_mpi_mt=$(cat conftest.out)
        ],
        [AC_MSG_FAILURE([
------------------------------
The execution of MPI multithread support test failed
------------------------------])
        ],
        [
          # Cross compilation mode
          ac_cv_mpi_mt=skip
          break;
        ])
      ])
  ])dnl

  AS_IF([test $ac_cv_mpi_mt = skip],[
    AC_MSG_WARN([
------------------------------
Multithreading support check was not done
because cross-compilation mode was detected.
------------------------------])
  ],[
    AS_IF([test $ac_cv_mpi_mt != concurrent],[
      AC_MSG_FAILURE([
------------------------------
MPI library specified does not support multithreading.
Please, provide a MPI library that does so.
Maximun multithread level supported: $ac_cv_mpi_mt
------------------------------])
    ])dnl
  ])dnl

  AS_CASE([$LIBS],
    [*"-lmpichcxx "*"-lmpi_mt"*],[
                         mpi_implementation=intel],
    [*"-lmpichcxx "*"-lmpich"*], [
                         mpi_implementation=mpich
                         CPPFLAGS=""],
    [*"-lmpicxx "*"-lmpi"*],     [
                         mpi_implementation=openmpi],
    [mpi_implementation=none]
  )
  AC_DEFINE_UNQUOTED([MPI_IMPLEMENTATION],[$mpi_implementation],
    [Identifies which MPI implementation is being used. Supported values: intel, mpich, openmpi])

  AC_SUBST([mpiflags])

  # Restore variables to its original state except CXXFLAGS
  # CXXFLAGS variable must include Intel MPI multithreading flag
  # Otherwise, libtool will use sequential version libmpi.so as MPICXX requirement
  # and link will fail
  AX_VAR_POPVALUE([CPPFLAGS])
  AX_VAR_POPVALUE([LDFLAGS])
  AX_VAR_POPVALUE([LIBS])


  AC_LANG_POP([C++])

])dnl AX_CHECK_MPI

