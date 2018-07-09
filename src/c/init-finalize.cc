/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <cassert>
#include <dlfcn.h>
#include <mpi.h>
#include <stdarg.h>

#include "include/TAMPI.h"

#include "definitions.h"
#include "environment.h"
#include "symbols.h"

extern "C" {

int MPI_Init(int *argc, char*** argv)
{
	static C::MPI_Init_t *symbol = (C::MPI_Init_t *) Symbol::loadNextSymbol(__func__);
	
	// Disable the interoperability
	C::Environment::disable();
	
	// Call MPI_Init
	return (*symbol)(argc, argv);
}

int MPI_Init_thread(int *argc, char ***argv, int required, int *provided)
{
	static C::MPI_Init_thread_t *symbol = (C::MPI_Init_thread_t *) Symbol::loadNextSymbol(__func__);
	
	/* NOTE: This should be modified when MPI_TASK_MULTIPLE
 	 * mode is supported by MPI */
	bool enableInterop = false;
	if (required == MPI_TASK_MULTIPLE) {
		required = MPI_THREAD_MULTIPLE;
		enableInterop = true;
	}
	
	// Call MPI_Init_thread
	int err = (*symbol)(argc, argv, required, provided);
	
	if (enableInterop && *provided == MPI_THREAD_MULTIPLE) {
		C::Environment::enable();
		C::Environment::initialize();
		*provided = MPI_TASK_MULTIPLE;
	} else {
		C::Environment::disable();
	}
	
	return err;
}

int MPI_Finalize()
{
	static C::MPI_Finalize_t *symbol = (C::MPI_Finalize_t *) Symbol::loadNextSymbol(__func__);
	
	// Call MPI_Finalize
	int err = (*symbol)();
	
	// Finalize the environment
	if (C::Environment::isEnabled()) {
		C::Environment::finalize();
	}
	
	return err;
}

} // extern C
