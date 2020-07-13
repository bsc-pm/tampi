/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2020 Barcelona Supercomputing Center (BSC)
*/

#include <config.h>

#include <cassert>
#include <dlfcn.h>
#include <mpi.h>
#include <stdarg.h>

#include "include/TAMPI_Decl.h"

#include "Definitions.hpp"
#include "Environment.hpp"
#include "Symbol.hpp"

extern "C" {

int MPI_Init(int *argc, char*** argv)
{
	static MPI_Init_t *symbol = (MPI_Init_t *) Symbol::load(__func__);

	// Disable both TAMPI modes
	Environment<C>::initialize(false, false);

	// Call MPI_Init
	return (*symbol)(argc, argv);
}

int MPI_Init_thread(int *argc, char ***argv, int required, int *provided)
{
	static MPI_Init_thread_t *symbol = (MPI_Init_thread_t *) Symbol::load(__func__);

	// Assuming that MPI does not provide MPI_TASK_MULTIPLE
	int irequired = required;
	if (required == MPI_TASK_MULTIPLE) {
		irequired = MPI_THREAD_MULTIPLE;
	}

	// Call MPI_Init_thread
	int err = (*symbol)(argc, argv, irequired, provided);
	if (err != MPI_SUCCESS) return err;

	bool enableBlockingMode = false;
	bool enableNonBlockingMode = false;
	if (*provided == MPI_THREAD_MULTIPLE) {
#if HAVE_BLOCKING_MODE
		if (required == MPI_TASK_MULTIPLE) {
			enableBlockingMode = true;
		}
#endif
#if HAVE_NONBLOCKING_MODE
		enableNonBlockingMode = true;
#endif
	}

	Environment<C>::initialize(enableBlockingMode, enableNonBlockingMode);
	if (enableBlockingMode) {
		*provided = MPI_TASK_MULTIPLE;
	}

	return MPI_SUCCESS;
}

int MPI_Finalize()
{
	static MPI_Finalize_t *symbol = (MPI_Finalize_t *) Symbol::load(__func__);

	// Call MPI_Finalize
	int err = (*symbol)();
	if (err != MPI_SUCCESS) return err;

	// Finalize the environment
	Environment<C>::finalize();

	return err;
}

} // extern C
