#	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
#
#	Copyright (C) 2022-2023 Barcelona Supercomputing Center (BSC)

AC_DEFUN([AC_CHECK_OVNI],
	[
		AC_ARG_WITH(
			[ovni],
			[AS_HELP_STRING([--with-ovni@<:@=DIR@:>@], [specify the installation prefix of the ovni library])],
			[ ac_use_ovni_prefix="${withval}" ],
			[ ac_use_ovni_prefix="no" ]
		)

		ovni_LIBS=""
		ovni_CPPFLAGS=""
		ac_use_ovni=no

		AC_MSG_CHECKING([the ovni installation prefix])
		if test x"${ac_use_ovni_prefix}" = x"no" ; then
			AC_MSG_RESULT([not enabled])
		elif test x"${ac_use_ovni_prefix}" = x"" ; then
			AC_MSG_RESULT([invalid prefix])
			AC_MSG_ERROR([ovni prefix specified but empty])
		else
			AC_MSG_RESULT([${ac_use_ovni_prefix}])

			if test x"${ac_use_ovni_prefix}" != x"yes" ; then
				ovni_CPPFLAGS="-I${ac_use_ovni_prefix}/include"
				ovni_LIBS="-L${ac_use_ovni_prefix}/lib"
			fi

			ac_save_CPPFLAGS="${CPPFLAGS}"
			ac_save_LIBS="${LIBS}"

			CPPFLAGS="${CPPFLAGS} ${ovni_CPPFLAGS}"
			LIBS="${LIBS} ${ovni_LIBS}"

			AC_CHECK_HEADERS([ovni.h], [], [AC_MSG_ERROR([ovni ovni.h header file not found])])
			AC_CHECK_LIB([ovni],
				[ovni_proc_init],
				[
					ovni_LIBS="${ovni_LIBS} -lovni"
					if test x"${ac_use_ovni_prefix}" != x"yes" ; then
						ovni_LIBS="${ovni_LIBS} -Wl,--enable-new-dtags -Wl,-rpath=${ac_use_ovni_prefix}/lib"
					fi
				],
				[AC_MSG_ERROR([ovni cannot be found])],
				[${ac_save_LIBS}]
			)

			ac_use_ovni=yes

			CPPFLAGS="${ac_save_CPPFLAGS}"
			LIBS="${ac_save_LIBS}"
		fi

		AM_CONDITIONAL(HAVE_OVNI, test x"${ac_use_ovni}" = x"yes")
		if test x"${ac_use_ovni}" = x"yes" ; then
			AC_DEFINE([HAVE_OVNI], [1], [Define if ovni is enabled.])
		fi


		AC_SUBST([ovni_LIBS])
		AC_SUBST([ovni_CPPFLAGS])
	]
)
