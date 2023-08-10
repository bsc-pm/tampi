#	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
#
#	Copyright (C) 2022-2023 Barcelona Supercomputing Center (BSC)

AC_DEFUN([AC_CHECK_OVNI],
	[
		AC_ARG_WITH(
			[ovni],
			[AS_HELP_STRING([--with-ovni@<:@=DIR@:>@], [enable ovni and specify the ovni library installation prefix])],
			[ac_use_ovni_prefix="${withval}"],
			[ac_use_ovni_prefix="no"]
		)

		ovni_LIBS=""
		ovni_CPPFLAGS=""
		ac_use_ovni=no

		AC_MSG_CHECKING([for the ovni installation])
		if test x"${ac_use_ovni_prefix}" = x"no" ; then
			AC_MSG_RESULT([not enabled])
		elif test x"${ac_use_ovni_prefix}" = x"" ; then
			AC_MSG_RESULT([invalid prefix])
			AC_MSG_ERROR([ovni prefix specified but empty])
		elif test x"${ac_use_ovni_prefix}" = x"yes" ; then
			AC_MSG_RESULT([enabled with no prefix])

			# Check ovni header and library without adding any prefix
			AC_CHECK_HEADERS([ovni.h], [], [AC_MSG_ERROR([ovni ovni.h header file not found])])
			AC_CHECK_LIB([ovni], [ovni_proc_init],
				[ac_use_ovni=yes],
				[AC_MSG_ERROR([libovni library not found])],
				[${LIBS}]
			)

			ovni_LIBS="-lovni"
		else
			AC_MSG_RESULT([${ac_use_ovni_prefix}])

			ac_save_CPPFLAGS="${CPPFLAGS}"
			ac_save_LIBS="${LIBS}"

			# Check ovni header
			ovni_CPPFLAGS="-I${ac_use_ovni_prefix}/include"
			CPPFLAGS="${ac_save_CPPFLAGS} ${ovni_CPPFLAGS}"

			AC_CHECK_HEADERS([ovni.h], [], [AC_MSG_ERROR([ovni ovni.h header file not found])])

			# Check ovni library in lib and lib64 subdirectories
			for libsubdir in lib lib64 ; do
				# Avoid the cached results of previous AC_CHECK_LIB
				AS_UNSET(ac_cv_lib_ovni_ovni_proc_init)

				LIBS="${ac_save_LIBS} -L${ac_use_ovni_prefix}/${libsubdir}"

				AC_CHECK_LIB([ovni], [ovni_proc_init], [ac_use_ovni=yes], [], [${ac_save_LIBS}])

				# Stop searching if found
				if test x"${ac_use_ovni}" = x"yes" ; then
					ac_use_ovni_libsubdir="${libsubdir}"
					break
				fi
			done

			if test x"${ac_use_ovni}" != x"yes" ; then
				AC_MSG_ERROR([libovni library not found])
			fi

			ovni_LIBS="-lovni -L${ac_use_ovni_prefix}/${ac_use_ovni_libsubdir}"
			onvi_LIBS="${ovni_LIBS} -Wl,--enable-new-dtags -Wl,-rpath=${ac_use_ovni_prefix}/${ac_use_ovni_libsubdir}"

			CPPFLAGS="${ac_save_CPPFLAGS}"
			LIBS="${ac_save_LIBS}"
		fi

		AM_CONDITIONAL(HAVE_OVNI, test x"${ac_use_ovni}" = x"yes")
		if test x"${ac_use_ovni}" = x"yes" ; then
			AC_DEFINE([HAVE_OVNI], [1], [Define if ovni is enabled])
		fi

		AC_SUBST([ovni_LIBS])
		AC_SUBST([ovni_CPPFLAGS])
	]
)
