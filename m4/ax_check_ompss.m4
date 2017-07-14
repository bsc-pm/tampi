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
AC_ARG_WITH(runtime,
[AS_HELP_STRING([--with-runtime=nanox|nanos6],
		[force compilation for a specific runtime])])

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

AC_MSG_CHECKING([which runtime to target])
case "x${with_runtime}" in
	"xnanox")
		AC_MSG_RESULT([nanox])
		AC_CHECK_HEADERS([nanox/nanos.h], [ompss=yes; break], [ompss=no])
  		search_lib=nanox-c
  		required_libs="-lnanox -lnanox-ompss"
		;;
	"xnanos6")
		AC_MSG_RESULT([nanos6])
		AC_CHECK_HEADERS([nanos6.h], [ompss=yes; break], [ompss=no])
		search_lib=nanos6
		required_libs=
		;;
	"x")
		AC_MSG_RESULT([try nanox then nanos6])
		AC_CHECK_HEADERS([nanox/nanos.h nanos6.h], [ompss=yes; break], [ompss=no])
		;;
	*)
		AC_MSG_ERROR([unknown runtime ${with_runtime}])
		;;
esac


# Tests if provided headers and libraries are usable and correct
AX_VAR_PUSHVALUE([CPPFLAGS],[$CPPFLAGS $ompssinc])
AX_VAR_PUSHVALUE([CXXFLAGS])
AX_VAR_PUSHVALUE([LDFLAGS],[$LDFLAGS $ompsslib])
AX_VAR_PUSHVALUE([LIBS],[])

m4_foreach([function],
           [nanos_register_polling_service,
            nanos_unregister_polling_service,
            nanos_get_current_blocking_context,
            nanos_block_current_task,
            nanos_unblock_task],
           [
             AS_IF([test "$ompss" = "yes"],[
               AC_SEARCH_LIBS(function,
                              [$search_lib],
                              [ompss=yes],
                              [ompss=no],
                              [$required_libs])dnl
             ])
           ])dnl

ompsslibs=$LIBS

AX_VAR_POPVALUE([CPPFLAGS])
AX_VAR_POPVALUE([CXXFLAGS])
AX_VAR_POPVALUE([LDFLAGS])
AX_VAR_POPVALUE([LIBS])

AS_IF([test "$ompss" != "yes"],[
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

