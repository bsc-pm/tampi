#
# SYNOPSIS
#
#   AX_COMPILE_FLAGS
#

AC_DEFUN([AX_COMPILE_FLAGS], [
	AC_ARG_ENABLE([debug-mode], [AS_HELP_STRING([--enable-debug-mode],
		[Adds compiler debug flags and enables additional internal debugging mechanisms [default=disabled]])])

	AS_IF([test "$enable_debug_mode" = yes],[
		# Debug mode is enabled
		tampi_CPPFLAGS=""
		tampi_CXXFLAGS="-O0 -g3 -Wall -Wextra"
	],[
		# Debug mode is enabled
		tampi_CPPFLAGS="-DNDEBUG"
		tampi_CXXFLAGS="-O3 -fno-rtti -fno-exceptions -fvisibility-inlines-hidden -Wall -Wextra"
	])

	AC_SUBST(tampi_CPPFLAGS)
	AC_SUBST(tampi_CXXFLAGS)

	# Disable autoconf default compilation flags
	: ${CPPFLAGS=""}
	: ${CXXFLAGS=""}
	: ${CFLAGS=""}
])
