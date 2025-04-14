#
# SYNOPSIS
#
#   AX_CHECK_CPUS
#
#   AX_CHECK_CPUS
#

AC_DEFUN([AX_CHECK_CPUS], [
	AC_MSG_CHECKING([the maximum number of CPUs])
	AC_ARG_WITH([max-cpus], [AS_HELP_STRING([--with-max-cpus=N/system], [Specify a custom value for the maximum number of CPUs in the system])], [ numcpus=$withval ], [ numcpus=system ])

	AS_IF([test "x$numcpus" = "xsystem"],[
			numcpus=$(nproc --all)
			AS_IF([numcpus=$(nproc --all 2> /dev/null)],[],
					[numcpus=$(grep -c processor /proc/cpuinfo 2> /dev/null)],[],
					[numcpus=$(sysctl -n hw.ncpu 2> /dev/null)],[],
					[numcpus=64])
		], [ ! [[[  "$numcpus" =~ ^[0-9]+$ ]]] ], [
			AC_MSG_ERROR([Invalid value for --with-max-cpus: $numcpus. It must be a positive integer or 'system'.])
		]
	)

	AC_SUBST([numcpus])
	AC_MSG_RESULT([${numcpus}])

	AC_DEFINE_UNQUOTED([MAX_SYSTEM_CPUS], [$numcpus], [Maximum number of CPUs])
])
