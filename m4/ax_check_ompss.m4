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
AC_PREREQ(2.59)dnl for _AC_LANG_PREFIX

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
if test "x$with_ompss" != xno; then
  ompss_prefix=$with_ompss
  ompssinc="$ompss_prefix/include"
  ompsslib=$ompss_prefix/lib
fi
if test "x$with_ompss_include" != x; then
  ompssinc="$with_ompss_include"
fi
if test "x$with_ompss_lib" != x; then
  ompsslib="$with_ompss_lib"
fi

# This is fulfilled even if $with_ompss="yes" 
# This happens when user leaves --with-value alone
if test x$with_ompss != xno; then

    #tests if provided headers and libraries are usable and correct
    bak_CFLAGS="$CFLAGS"
    bak_CPPFLAGS="$CPPFLAGS"
    bak_LIBS="$LIBS"
    bak_LDFLAGS="$LDFLAGS"

    CFLAGS=
    CPPFLAGS=-I$ompssinc
    LIBS=
    LDFLAGS=-L$ompsslib

    # Check if nanos.h header file exists and compiles
    AC_CHECK_HEADER([nanox/nanos.h], [ompss=yes],[ompss=no])

    # Look for nanox_polling_cond_wait function in libnanox-c.so library
    if test x$ompss == xyes; then
      AC_CHECK_LIB([nanox-c],
                     [nanos_polling_cond_wait],
                     [ompss=yes
                      LIBS="$LIBS $ompsslib/libnanox-c.la"],
                     [ompss=no])
    fi

    ompssinc=-I$ompssinc
    ompsslibs=$LIBS

    CFLAGS="$bak_CFLAGS"
    CPPFLAGS="$bak_CPPFLAGS"
    LIBS="$bak_LIBS"
    LDFLAGS="$bak_LDFLAGS"

    if test x$ompss != xyes; then
        AC_MSG_ERROR([
------------------------------
ompss path was not correctly specified. 
Please, check that the provided directories are correct.
------------------------------])
    fi
fi

AC_SUBST([ompss])
AC_SUBST([ompss_prefix])
AC_SUBST([ompssinc])
AC_SUBST([ompsslib])
AC_SUBST([ompsslibs])

])dnl AX_CHECK_OMPSS

