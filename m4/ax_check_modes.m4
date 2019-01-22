#
# SYNOPSIS
#
#   AX_CHECK_BLOCKING_MODE
#
#   AX_CHECK_NONBLOCKING_MODE
#

AC_DEFUN([AX_CHECK_BLOCKING_MODE], [
	AC_MSG_CHECKING([whether the blocking mode of TAMPI is enabled])
	AC_ARG_ENABLE(
		[blocking-mode],
		[AS_HELP_STRING([--disable-blocking-mode], [do not compile the blocking mode of TAMPI [default=enabled]])],
		[
			case "${enableval}" in
			yes)
				AC_MSG_RESULT([yes])
				ac_blocking_mode=yes
				;;
			no)
				AC_MSG_RESULT([no])
				ac_blocking_mode=no
				;;
			*)
				AC_MSG_ERROR([bad value ${enableval} for --disable-blocking-mode])
				;;
			esac
		],
		[
			AC_MSG_RESULT([yes])
			ac_blocking_mode=yes
		]
	)
	if test x"${ac_blocking_mode}" = x"no" ; then
		AC_DEFINE([HAVE_BLOCKING_MODE], 0, [Compile the blocking mode of TAMPI])
	else
		AC_DEFINE([HAVE_BLOCKING_MODE], 1, [Compile the blocking mode of TAMPI])
	fi
])

AC_DEFUN([AX_CHECK_NONBLOCKING_MODE], [
	AC_MSG_CHECKING([whether the non-blocking mode of TAMPI is enabled])
	AC_ARG_ENABLE(
		[nonblocking-mode],
		[AS_HELP_STRING([--disable-nonblocking-mode], [do not compile the non-blocking mode of TAMPI [default=enabled]])],
		[
			case "${enableval}" in
			yes)
				AC_MSG_RESULT([yes])
				ac_nonblocking_mode=yes
				;;
			no)
				AC_MSG_RESULT([no])
				ac_nonblocking_mode=no
				;;
			*)
				AC_MSG_ERROR([bad value ${enableval} for --disable-nonblocking-mode])
				;;
			esac
		],
		[
			AC_MSG_RESULT([yes])
			ac_nonblocking_mode=yes
		]
	)
	if test x"${ac_nonblocking_mode}" = x"no" ; then
		AC_DEFINE([HAVE_NONBLOCKING_MODE], 0, [Compile the non-blocking mode of TAMPI])
	else
		AC_DEFINE([HAVE_NONBLOCKING_MODE], 1, [Compile the non-blocking mode of TAMPI])
	fi
])

AC_DEFUN([AX_NOTIFY_EXTERNAL_EVENTS_API], [
	AC_MSG_CHECKING([whether TAMPI should notify the runtime system before using the external events API])
	AC_ARG_ENABLE(
		[external-events-api-notification],
		[AS_HELP_STRING([--enable-external-events-api-notification], [notify the runtime system during TAMPI's initialization for the possible use of the external events API [default=disabled]])],
		[
			case "${enableval}" in
			yes)
				AC_MSG_RESULT([yes])
				ac_external_events_api_notification=yes
				;;
			no)
				AC_MSG_RESULT([no])
				ac_external_events_api_notification=no
				;;
			*)
				AC_MSG_ERROR([bad value ${enableval} for --enable-external-events-api-notification])
				;;
			esac
		],
		[
			AC_MSG_RESULT([no])
			ac_external_events_api_notification=no
		]
	)
	if test x"${ac_external_events_api_notification}" = x"no" ; then
		AC_DEFINE([NOTIFY_EXTERNAL_EVENTS_API], 0, [Notify the runtime before using the external events API])
	else
		AC_DEFINE([NOTIFY_EXTERNAL_EVENTS_API], 1, [Notify the runtime before using the external events API])
	fi
])
