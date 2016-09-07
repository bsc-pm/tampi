#
# SYNOPSIS
#
#   AX_CHECK_OMPSS
#
# DESCRIPTION
#
#   Check whether OmpSs path to the headers and libraries are correctly specified.
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

AC_DEFUN([AX_CHECK_OMPSS],[

#Check if an ompss implementation is installed.
AC_ARG_WITH(ompss,
[AS_HELP_STRING([--with-ompss,--with-ompss=PATH],
                [search in system directories or specify prefix directory for installed ompss package])])
AC_ARG_WITH(ompss-include,
[AS_HELP_STRING([--with-ompss-include=PATH],
                [specify directory for installed ompss include files])])
AC_ARG_WITH(ompss-lib,
[AS_HELP_STRING([--with-ompss-lib=PATH],
                [specify directory for the installed ompss library])])

# Search for ompss by default
AS_IF([test "$with_ompss" != yes],[
  ompssinc="-I$with_ompss/include"
  AS_IF([test -d $with_ompss/lib64],
    [olibdir=$with_ompss/lib64],
    [olibdir=$with_ompss/lib])

  ompsslib="-L$olibdir -Wl,-rpath,$olibdir"
])

AS_IF([test "x$with_ompss_include" != x],[
  ompssinc="-I$with_ompss_include"
])
AS_IF([test "x$with_ompss_lib" != x],[
  ompsslib="-L$with_ompss_lib -Wl,-rpath,$with_ompss_lib"
])

# Tests if provided headers and libraries are usable and correct
AX_VAR_PUSHVALUE([CPPFLAGS],[$CPPFLAGS $ompssinc])
AX_VAR_PUSHVALUE([CXXFLAGS])
AX_VAR_PUSHVALUE([LDFLAGS],[$LDFLAGS $ompsslib])
AX_VAR_PUSHVALUE([LIBS],[])

# Check if nanos runtime header file exists and compiles
AC_CHECK_HEADERS([nanox/nanos.h nanos6/nanos6_rt_interface.h], 
                 [ompss=yes; break],
                 [ompss=no])

AS_IF([test "$ac_cv_header_nanox_nanos_h" == yes],
        [runtime_version=5
         ompssinc="$ompssinc/nanox $ompssinc/nanox-dev"
         prereq_libs="-lnanox-ompss"
        ],
      [test "$ac_cv_header_nanos6_nanos6_rt_interface_h" == yes],
        [runtime_version=6])

# Look for nanos_register_polling_service function in OmpSs runtime libraries
AC_SEARCH_LIBS([nanos_register_polling_service],
  [nanox-c nanos6-argobots nanos6],
  [ompss=yes],
  [ompss=no],
  [$prereq_libs])

ompsslibs=$LIBS

AX_VAR_POPVALUE([CPPFLAGS])
AX_VAR_POPVALUE([CXXFLAGS])
AX_VAR_POPVALUE([LDFLAGS])
AX_VAR_POPVALUE([LIBS])

AS_IF([test "$ompss" != "yes" -a "$runtime_version" != ""],[
    AC_MSG_ERROR([
------------------------------
OmpSs path was not correctly specified. 
Please, check that the provided directories are correct.
------------------------------])
])

AC_SUBST([ompss])
AC_SUBST([ompssinc])
AC_SUBST([ompsslib])
AC_SUBST([ompsslibs])

])dnl AX_CHECK_OMPSS

