#
# SYNOPSIS
#
#   AX_CHECK_CXX_VERSION
#
#   AX_COMPILE_FLAGS
#

AC_DEFUN([AX_CHECK_CXX_VERSION], [
	AC_MSG_CHECKING([the ${CXX} version])
	if test x"$CC" != x"" ; then
		CXX_VERSION=$(${CXX} --version | head -1)
	fi
	AC_MSG_RESULT([$CXX_VERSION])
	AC_SUBST([CXX_VERSION])
])

AC_DEFUN([AX_COMPILE_FLAGS], [
	AC_ARG_ENABLE([debug-mode], [AS_HELP_STRING([--enable-debug-mode],
		[Adds compiler debug flags and enables additional internal debugging mechanisms @<:@default=disabled@:>@])])

	AS_IF([test "$enable_debug_mode" = yes],[
		# Debug mode is enabled
		tampi_CPPFLAGS=""
		tampi_CXXFLAGS="-Wall -Wextra -Wshadow -fvisibility=hidden -O0 -g3"
	],[
		# Debug mode is enabled
		tampi_CPPFLAGS="-DNDEBUG"
		tampi_CXXFLAGS="-Wall -Wextra -Wshadow -fvisibility=hidden -O3"
	])

	AC_SUBST(tampi_CPPFLAGS)
	AC_SUBST(tampi_CXXFLAGS)

	# Disable autoconf default compilation flags
	: ${CPPFLAGS=""}
	: ${CXXFLAGS=""}
	: ${CFLAGS=""}
])
