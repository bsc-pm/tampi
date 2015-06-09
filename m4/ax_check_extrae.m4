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
[AS_HELP_STRING([--with-extrae,--with-ompss=PATH],
                [search in system directories or specify prefix directory for installed extrae package])])
AC_ARG_WITH(extrae-include,
[AS_HELP_STRING([--with-extrae-include=PATH],
                [specify directory for installed extrae include files])])
AC_ARG_WITH(extrae-lib,
[AS_HELP_STRING([--with-extrae-lib=PATH],
                [specify directory for the installed extrae library])])

# Search for extrae by default
if [[[ ! "x$with_extrae" =~  x(yes|no|)$ ]]]; then
  extraeinc="$with_extrae/include"
  extraelib=$with_extrae/lib
fi
if test "x$with_extrae_lib" != x; then
  extraelib="$with_extrae_lib"
fi

# This is fulfilled even if $with_extrae="yes" 
# This happens when user leaves --with-value alone
if test x$extraelib != x; then

    #tests if provided headers and libraries are usable and correct
    bak_CFLAGS="$CFLAGS"
    bak_CPPFLAGS="$CPPFLAGS"
    bak_LIBS="$LIBS"
    bak_LDFLAGS="$LDFLAGS"

    CFLAGS=
    CPPFLAGS=-I$extraeinc
    LIBS=
    LDFLAGS=-L$extraelib

    # Looking for MPI_Isend function in libnanosmpitrace library
    # is complex due to the variable number of dependencies it has,
    # so we just check for the existence of the 
    AC_MSG_CHECKING([extrae libraries])
    if [[ -e $extraelib/libnanosmpitrace.la ]]; then
       AC_MSG_RESULT([found])
       extrae=yes

       LIBS="$LIBS $extraelib/libnanosmpitrace.la"
    else
       AC_MSG_RESULT([not found])
       extrae=no
    fi

    extraelibs=$LIBS

    CFLAGS="$bak_CFLAGS"
    CPPFLAGS="$bak_CPPFLAGS"
    LIBS="$bak_LIBS"
    LDFLAGS="$bak_LDFLAGS"

    if test x$extrae != xyes; then
        AC_MSG_ERROR([
------------------------------
Either provided extrae path was not correctly specified,
or installation is not correct. 
------------------------------])
    fi
fi

AC_SUBST([extrae])
AC_SUBST([extrae_prefix])
AC_SUBST([extraeinc])
AC_SUBST([extraelib])
AC_SUBST([extraelibs])
AM_CONDITIONAL([USE_EXTRAE],[test x$extrae = xyes])

])dnl AX_CHECK_EXTRAE

