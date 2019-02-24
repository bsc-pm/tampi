#
# SYNOPSIS
#
#   AX_CHECK_CPUS
#
#   AX_CHECK_CPUS
#

AC_DEFUN([AX_CHECK_CPUS], [
	AC_MSG_CHECKING([the maximum number of CPUs in the system])
	numcpus=$(nproc --all)
	AS_IF([numcpus=$(nproc --all 2> /dev/null)],[],
			[numcpus=$(grep -c processor /proc/cpuinfo 2> /dev/null)],[],
			[numcpus=$(sysctl -n hw.ncpu 2> /dev/null)],[],
			[numcpus=50])
	AC_SUBST([numcpus])
	AC_MSG_RESULT([${numcpus}])
	
	AC_DEFINE_UNQUOTED([MAX_SYSTEM_CPUS], [$numcpus], [Maximum number of CPUs in the system])
])
