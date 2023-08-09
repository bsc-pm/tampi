#
# SYNOPSIS
#
#   AX_CHECK_CXX_VERSION
#
#   AX_COMPILE_FLAGS
#

AC_DEFUN([AX_CHECK_CXX_VERSION], [
	AC_MSG_CHECKING([the ${CXX} version])
	if test x"$CXX" != x"" ; then
		CXX_VERSION=$(${CXX} --version | head -1)
	fi
	AC_MSG_RESULT([$CXX_VERSION])
	AC_SUBST([CXX_VERSION])
])

AC_DEFUN([AX_COMPILE_FLAGS], [
	# Disable debug and address sanitizer by default
	ac_enable_debug=no
	ac_enable_asan=no

	AC_ARG_ENABLE([debug], [AS_HELP_STRING([--enable-debug],
		[Adds compiler debug flags and enables additional internal debugging mechanisms @<:@default=disabled@:>@])],
		[ ac_enable_debug=$enableval ])

	AC_ARG_ENABLE([debug-mode], [AS_HELP_STRING([--enable-debug-mode],
		[Deprecated option to enable debug; use --enable-debug instead @<:@default=disabled@:>@])],
		[
			AC_MSG_WARN([Option --enable-debug-mode is deprecated; use --enable-debug instead])
			ac_enable_debug=$enableval
		])

	AC_ARG_ENABLE([asan], [AS_HELP_STRING([--enable-asan],
		[Enables address sanitizing @<:@default=disabled@:>@])],
		[ ac_enable_asan=$enableval ])

	if test x"${ac_enable_debug}" = x"yes" ; then
		# Use debug flags
		tampi_CPPFLAGS=""
		tampi_CXXFLAGS="-Wall -Wextra -Wshadow -fvisibility=hidden -O0 -g3"
	else
		# Use optimized flags
		tampi_CPPFLAGS="-DNDEBUG"
		tampi_CXXFLAGS="-Wall -Wextra -Wshadow -fvisibility=hidden -O3"
	fi

	if test x"${ac_enable_asan}" = x"yes" ; then
		if test x"${ac_enable_debug}" = x"no" ; then
			AC_MSG_WARN([Enabling address sanitizer without enabling debug is not recommended])
		fi

		# Enable address sanitizer flags
		ac_asan_flags="-fsanitize=address -fno-omit-frame-pointer"
		asan_CPPFLAGS="${ac_asan_flags}"
		asan_CXXFLAGS="${ac_asan_flags}"
		asan_LDFLAGS="${ac_asan_flags}"
	else
		asan_CPPFLAGS=""
		asan_CXXFLAGS=""
		asan_LDFLAGS=""
	fi

	AC_SUBST(tampi_CPPFLAGS)
	AC_SUBST(tampi_CXXFLAGS)

	AC_SUBST(asan_CPPFLAGS)
	AC_SUBST(asan_CXXFLAGS)
	AC_SUBST(asan_LDFLAGS)

	# Disable autoconf default compilation flags
	: ${CPPFLAGS=""}
	: ${CXXFLAGS=""}
	: ${CFLAGS=""}
	: ${LDFLAGS=""}
])
