/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <dlfcn.h>
#include <mpi.h>

#include "definitions.h"
#include "environment.h"
#include "symbols.h"

extern "C" {
	int MPI_Query_thread(int *provided)
	{
		int err = MPI_SUCCESS;
		if (C::Environment::isEnabled()) {
			assert(provided != NULL);
			*provided = MPI_TASK_MULTIPLE;
		} else {
			static C::MPI_Query_thread_t *symbol = (C::MPI_Query_thread_t *) Symbol::loadNextSymbol(__func__);
			err = (*symbol)(provided);
		}
		return err;
	}
} // extern C

