#
# SYNOPSIS
#
#   AX_DEBUG_MODE
#

AC_DEFUN([AX_DEBUG_MODE], [

  AC_ARG_ENABLE([debug-mode], [AS_HELP_STRING([--enable-debug-mode=yes/no], 
    [Adds compiler debug flags and enables additional internal debugging mechanisms.])])

  AS_IF([test "$enable_debug_mode" = yes],[
    # Debug mode is enabled
    AC_DEFINE([DEBUG_MODE],[1],[Activates debug sections of the library.])
: ${CXXFLAGS="-O0 -g3"}
  ],[
    # Debug mode is disabled. assert() macro is turned off.
    AC_DEFINE([NDEBUG],[1],[Disables assert() macro.])
  ])

  AM_CONDITIONAL([debug_mode_enabled],[test "$enable_debug_mode" = yes])
])
