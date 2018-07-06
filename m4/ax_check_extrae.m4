#
# SYNOPSIS
#
#   AX_CHECK_EXTRAE
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

AC_DEFUN([AX_CHECK_EXTRAE],[
AC_PREREQ(2.59)dnl for _AC_LANG_PREFIX

#Check if an extrae implementation is installed.
AC_ARG_WITH(extrae,
[AS_HELP_STRING([--with-extrae,--with-extrae=PATH],
                [search in system directories or specify prefix directory for installed extrae package])])
AC_ARG_WITH(extrae-include,
[AS_HELP_STRING([--with-extrae-include=PATH],
                [specify directory for installed extrae include files])])
AC_ARG_WITH(extrae-lib,
[AS_HELP_STRING([--with-extrae-lib=PATH],
                [specify directory for the installed extrae library])])

# Search for extrae by default
AS_IF([ [[[ ! "x$with_extrae" =~  x(yes|no|)$ ]]] ],[
  extraeinc="$with_extrae/include"
  extraelib=$with_extrae/lib
])
AS_IF([test "$with_extrae_lib" != ""],[
  extraelib="$with_extrae_lib"
])

# This is fulfilled even if $with_extrae="yes" 
# This happens when user leaves --with-value alone
AS_IF([test "$extraelib" != "" ],[

    extraeinc="-I$extraeinc"
    extraelib="-L$extraelib -Wl,-rpath,$extraelib"

    #tests if provided headers and libraries are usable and correct
    AX_VAR_PUSHVALUE([CPPFLAGS],[$CPPFLAGS $extraeinc])
    AX_VAR_PUSHVALUE([CXXFLAGS])
    AX_VAR_PUSHVALUE([LIBS])
    AX_VAR_PUSHVALUE([LDFLAGS],[$LDFLAGS $extraelib])

    # Looking for MPI_Isend function in libnanosmpitrace library
    # is complex due to the variable number of dependencies it has,
    # so we just check for the existence of the 
    AC_CHECK_HEADERS([extrae.h], 
      [extrae=yes],
      [extrae=no])

    m4_foreach([function],
      [Extrae_event,
       Extrae_eventandcounters],
      [
        AS_IF([test "$extrae" = "yes"],[
          AC_SEARCH_LIBS(function,
            [mpitrace nanosmpitrace],
            [extrae=yes],
            [extrae=no],
            [])
        ])
    ])dnl

    extraelibs=$LIBS

    AX_VAR_POPVALUE([CPPFLAGS])
    AX_VAR_POPVALUE([CXXFLAGS])
    AX_VAR_POPVALUE([LIBS])
    AX_VAR_POPVALUE([LDFLAGS])

    AS_IF([test "$extrae" != "yes"],[
        AC_MSG_ERROR([
------------------------------
Extrae path was not correctly specified
or installation is not correct. 
------------------------------])
    ])
])

AC_SUBST([extrae])
AC_SUBST([extrae_prefix])
AC_SUBST([extraeinc])
AC_SUBST([extraelib])
AC_SUBST([extraelibs])
AM_CONDITIONAL([USE_EXTRAE],[test x$extrae = xyes])

])dnl AX_CHECK_EXTRAE

