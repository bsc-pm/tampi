#
# SYNOPSIS
#
#   AX_CHECK_ARGOBOTS
#
# DESCRIPTION
#
#   Check whether Argobots path to the headers and libraries are correctly specified.
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

AC_DEFUN([AX_CHECK_ARGOBOTS],[

#Check if an argobots implementation is installed.
AC_ARG_WITH(argobots,
[AS_HELP_STRING([--with-argobots,--with-argobots=PATH],
                [search in system directories or specify prefix directory for installed argobots package])])

# Search for argobots by default
AS_IF([test "$with_argobots" != yes],[
  argobotsinc="-I$with_argobots/include"
  argobotslib="-L$with_argobots/lib -Wl,-rpath,$with_argobots/lib"
])

# This is fulfilled even if $with_argobots="yes" 
# This happens when user leaves --with-value alone
AS_IF([test "$runtime_version" -gt 5 -and "$with_argobots" != no],[
    #tests if provided headers and libraries are usable and correct
    AX_VAR_PUSHVALUE(CPPFLAGS,[$CPPFLAGS $argobotsinc])
    AX_VAR_PUSHVALUE(CXXFLAGS)
    AX_VAR_PUSHVALUE(LDFLAGS,[$LDFLAGS $argobotslib])
    AX_VAR_PUSHVALUE(LIBS,[])

    # Check if nanos.h header file exists and compiles
    AC_CHECK_HEADERS([abt.h], [argobots=yes],[argobots=no])

    # Look for nanox_polling_cond_wait function in libnanox-c.so library
    AS_IF([test "$argobots" == yes],[
      AC_SEARCH_LIBS([ABT_eventual_create],
                   [abt],
                   [argobots=yes],
                   [argobots=no])
    ])

    argobotslibs=$LIBS

    AX_VAR_POPVALUE([CPPFLAGS])
    AX_VAR_POPVALUE([CXXFLAGS])
    AX_VAR_POPVALUE([LDFLAGS])
    AX_VAR_POPVALUE([LIBS])

    AS_IF([test "$argobots" == yes],[
      AC_DEFINE([HAVE_ARGOBOTS],[],[Argobots ULT library is available.])
    ],[
        AC_MSG_ERROR([
------------------------------
Argobots path was not correctly specified. 
Please, check that the provided directories are correct.
------------------------------])
    ])
])

AC_SUBST([argobots])
AC_SUBST([argobotsinc])
AC_SUBST([argobotslib])
AC_SUBST([argobotslibs])
AM_CONDITIONAL([nanos_argobots],[test "$argobots" == yes])

])dnl AX_CHECK_ARGOBOTS

